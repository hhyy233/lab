#include "MemManUnit.h"


MemManUnit::MemManUnit() {}

MemManUnit::MemManUnit(char algo, string options, int num, string fn) {
	frametable = vector<Frame>(num, Frame());
	switch (algo)
	{
	case 'f': pager = new FIFO_Pager(); break;
	case 's': pager = new SC_Pager(); break;
	case 'r': pager = new R_Pager(fn); break;
	case 'n': pager = new NRU_Pager(fn); break;
	case 'c': pager = new C_Pager(); break;
	case 'a': pager = new A_Pager(); break;
	}
	int n = options.length();
	O = P = F = S = f = a = x = y = false;
	for (int ii = 0; ii < n; ii++) {
		switch (options[ii])
		{
		case 'O':O = true; break;
		case 'P':P = true; break;
		case 'F':F = true; break;
		case 'S':S = true; break;
		case 'f':f = true; break;
		case 'a':a = true; break;
		case 'x':x = true; break;
		case 'y':y = true; break;
		}
	}
	curr_proc = -1;
	ctx_switches = inst_count = 0;
	cost = 0;
}

void MemManUnit::add_proc() {
	pagetable.push_back(vector<PTE>(64, PTE()));
	curr_proc = pagetable.size() - 1;
	pinf.push_back(proc_info(curr_proc));
}

void MemManUnit::initialize_page(int sp, int ep, int wp, int fm) {
	for (int ii = sp; ii <= ep; ii++) {
		pagetable[curr_proc][ii].setbit(wp, fm);
	}
}

void MemManUnit::process_inst(char opt, int opn) {
	if (O) {
		printf("%lu: ==> %c %d\n", inst_count, opt, opn);
	}
	switch (opt)
	{
	case 'c':context_switch(opn); break;
	case 'r':
	case 'w':ref_page(opt, opn); break;
	}
	//print pagetable
	if (y) {
		for (unsigned int ii = 0; ii < pagetable.size(); ii++) {
			print_pt(ii);
		}
	}
	else if (x) {
		print_pt(curr_proc);
	}
	if (f) {
		print_ft();
	}
	if (a) pager->print_info();
	inst_count++;
}

void MemManUnit::context_switch(int procid) {
	curr_proc = procid;
	ctx_switches++;
	cost += cost_cont_switch;
}

void MemManUnit::ref_page(char operation, int vpage) {
	cost += cost_readwrite;
	PTE* pte = &pagetable[curr_proc][vpage];
	//segv check
	if (!pte->ACCESSABLE) {
		cout << " SEGV" << endl;
		pinf[curr_proc].segv++;
		cost += cost_segv;
		return;
	}

	PTE* prev_pte;
	if (pte->PRESENT) {
		pte->REFERENCED = 1;
	}
	else {
		Frame* newframe = get_frame();
		int frameid, prev_procid, prev_vpage;
		frameid = newframe->FRAMEID;

		//do something
		if (!isfree) {
			prev_procid = newframe->PROCID;
			prev_vpage = newframe->VPAGE;
			prev_pte = &pagetable[prev_procid][prev_vpage];
			//unmap
			prev_pte->PRESENT = 0;
			pinf[prev_procid].unmaps++;
			//cost
			cost += cost_map_un;
			if (prev_pte->MODIFIED) {
				if (prev_pte->FILE_MAPPED) {
					pinf[prev_procid].fouts++;
					cost += cost_file_inout;
				}
				else {
					prev_pte->PAGEOUT = 1;
					pinf[prev_procid].outs++;
					cost += cost_page_inout;
				}
			}
			//print info
			if (O) {
				printf(" UNMAP %d:%d\n", prev_procid, prev_vpage);
				if (prev_pte->MODIFIED) {
					if (prev_pte->FILE_MAPPED) cout << " FOUT" << endl;
					else cout << " OUT" << endl;
				}
			}
		}

		//reset pte
		pte->PRESENT = 1;
		pte->REFERENCED = 0;
		pte->MODIFIED = 0;
		pte->FRAMEID = frameid;

		//process
		if (pte->FILE_MAPPED) {
			pinf[curr_proc].fins++;
			cost += cost_file_inout;
		}
		else if (pte->PAGEOUT) {
			pinf[curr_proc].ins++;
			cost += cost_page_inout;
		}
		else {
			pinf[curr_proc].zeros++;
			cost += cost_zero;
		}


		//output summary
		if (O) {

			if (pte->FILE_MAPPED) {
				cout << " FIN" << endl;
			}
			else if (pte->PAGEOUT) {
				cout << " IN" << endl;
			}
			else {
				cout << " ZERO" << endl;
			}
			printf(" MAP %d\n", frameid);
		}

		//frame map
		newframe->PROCID = curr_proc;
		newframe->VPAGE = vpage;
		pinf[curr_proc].maps++;
		cost += cost_map_un;
	}

	//segproc check
	pte->REFERENCED = 1;
	if (operation == 'w'&&pte->WRITE_PROTECT) {
		cout << " SEGPROT" << endl;
		pinf[curr_proc].segprot++;
		cost += cost_segprot;
		return;
	}
	else {//do operation
		if (operation == 'w') {
			pte->MODIFIED = 1;
		}
	}
}

Frame* MemManUnit::get_frame() {
	isfree = true;
	Frame* frame = allocate_frame_from_free_list();
	if (frame == NULL) {
		isfree = false;
		frame = pager->determine_victim_frame(pagetable, frame_list, frametable);
	}
	return frame;
}

Frame* MemManUnit::allocate_frame_from_free_list() {
	if (frame_list.size() == frametable.size()) return NULL;
	else {
		int pos = frame_list.size();
		Frame* frame = &frametable[pos];
		frame->FRAMEID = pos;
		frame_list.push_back(frame);
		return frame;
	}
}

void MemManUnit::print_pt(int procid) {
	printf("PT[%d]: ", procid);
	int n = pagetable[procid].size();
	PTE* pte;
	for (int ii = 0; ii < n; ii++) {
		pte = &pagetable[procid][ii];
		if (pte->PRESENT) {
			cout << ii << ':';
			if (pte->REFERENCED)cout << 'R';
			else cout << '-';
			if (pte->MODIFIED) cout << 'M';
			else cout << '-';
			if (pte->PAGEOUT) cout << 'S';
			else cout << '-';
		}
		else {
			if (pte->PAGEOUT) cout << '#';
			else cout << '*';
		}
		cout << ' ';
	}
	cout << endl;
}

void MemManUnit::print_ft() {
	unsigned int n = frame_list.size();
	Frame f;
	int procid, vpage;
	cout << "FT: ";
	for (unsigned int ii = 0; ii < n; ii++) {
		f = frametable[ii];
		procid = f.PROCID;
		vpage = f.VPAGE;
		printf("%d:%d ", procid, vpage);
	}
	if (n < frametable.size()) {
		for (unsigned int ii = n; ii < frametable.size(); ii++) {
			cout << "* " ;
		}
	}
	cout << endl;
}

void MemManUnit::print_sum() {
	int n = pinf.size();
	proc_info* pstats;
	for (int ii = 0; ii < n; ii++) {
		pstats = &pinf[ii];
		printf("PROC[%d]: U=%lu M=%lu I=%lu O=%lu FI=%lu FO=%lu Z=%lu SV=%lu SP=%lu\n",
			pstats->pid,
			pstats->unmaps, pstats->maps, pstats->ins, pstats->outs,
			pstats->fins, pstats->fouts, pstats->zeros,
			pstats->segv, pstats->segprot);
	}
	printf("TOTALCOST %lu %lu %llu\n", ctx_switches, inst_count, cost);
}

void MemManUnit::print_res() {
	if (P) {
		for (unsigned int ii = 0; ii < pagetable.size(); ii++) {
			print_pt(ii);
		}
	}
	if (F) print_ft();
	if (S) print_sum();
}