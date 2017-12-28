#include "Pager.h"
#include "Bit.h"

Pager::Pager() {}

int  Pager::myrand() {
	unsigned int randval;
	if (rfile.eof()) {
		rfile.seekg(0, ios::beg);
		rfile >> randval;
	}
	rfile >> randval;
	return randval;
}

void Pager::print_info() {}

FIFO_Pager::FIFO_Pager() {}

Frame* FIFO_Pager::determine_victim_frame(vector<vector<PTE>>& pagetable, list<Frame*>& frame_list, vector<Frame>& frametable) {
	Frame* f = frame_list.front();
	frame_list.pop_front();
	frame_list.push_back(f);
	return f;
}

SC_Pager::SC_Pager() {}

Frame* SC_Pager::determine_victim_frame(vector<vector<PTE>>& pagetable, list<Frame*>& frame_list, vector<Frame>& frametable) {
	bool flag = false;
	PTE* pte;
	Frame* f;
	unsigned int prev_procid, prev_vpage;
	do {
		f = frame_list.front();
		frame_list.pop_front();
		frame_list.push_back(f);
		prev_procid = f->PROCID;
		prev_vpage = f->VPAGE;
		pte = &pagetable[prev_procid][prev_vpage];
		if (pte->REFERENCED) {
			pte->REFERENCED = 0;
		}
		else {
			flag = true;
		}
	} while (!flag);
	return f;
}

R_Pager::R_Pager(string fn) {
	rfile.open(fn);
	unsigned int tmp;
	rfile >> tmp;
}

Frame* R_Pager::determine_victim_frame(vector<vector<PTE>>& pagetable, list<Frame*>& frame_list, vector<Frame>& frametable) {
	unsigned int n = frame_list.size();
	unsigned int pos = myrand() % n;
	Frame* f = &frametable[pos];
	return f;
}

NRU_Pager::NRU_Pager(string fn) {
	rfile.open(fn);
	unsigned int tmp;
	rfile >> tmp;
	count = 0;
}

Frame* NRU_Pager::determine_victim_frame(vector<vector<PTE>>& pagetable, list<Frame*>& frame_list, vector<Frame>& frametable) {
	Frame* f;
	vector<vector<unsigned int>> prio(4, vector<unsigned int>());
	unsigned int pid, vp;
	
	int R, M;
	for (unsigned int ii = 0; ii < frametable.size(); ii++) {
		pid = frametable[ii].PROCID;
		vp = frametable[ii].VPAGE;
		R = pagetable[pid][vp].REFERENCED;
		M = pagetable[pid][vp].MODIFIED;
		if (R == 0 && M == 0) prio[0].push_back(ii);
		else if (R == 0 && M == 1)prio[1].push_back(ii);
		else if (R == 1 && M == 0) prio[2].push_back(ii);
		else prio[3].push_back(ii);
	}
	unsigned int pos=0;
	while (prio[pos].empty()) pos++;
	unsigned int fid = prio[pos][myrand() % prio[pos].size()];
	f = &frametable[fid];

	count++;//WTF??
	if (count == 10) {
		count = 0;
		for (unsigned int ii = 0; ii < frametable.size(); ii++) {
			pid = frametable[ii].PROCID;
			vp = frametable[ii].VPAGE;
			pagetable[pid][vp].REFERENCED = 0;
		}
	}

	return f;
}

C_Pager::C_Pager(){
	pointer = 0;
}

Frame* C_Pager::determine_victim_frame(vector<vector<PTE>>& pagetable, list<Frame*>& frame_list, vector<Frame>& frametable) {
	Frame* f;
	unsigned int n = frametable.size();
	int R;
	unsigned int pid, vp;
	do {
		f = &frametable[pointer];
		pid = f->PROCID;
		vp = f->VPAGE;
		R = pagetable[pid][vp].REFERENCED;
		pagetable[pid][vp].REFERENCED = 0;
		pointer = (pointer + 1) % n;
	} while (R);
	return f;
}

A_Pager::A_Pager(){

}

Frame* A_Pager::determine_victim_frame(vector<vector<PTE>>& pagetable, list<Frame*>& frame_list, vector<Frame>& frametable) {
	unsigned int n = frame_list.size();
	if (counter.size() == 0) counter = vector<unsigned int>(n, 0);

	unsigned int mask = 1;
	mask <<= 31;
	unsigned int lowest_c = -1;//record the lowest counter
	unsigned int lowest_p = 0;//record the position

	Frame* f;
	unsigned int pid, vp;
	for (unsigned int ii = 0; ii < n; ii++) {
		pid = frametable[ii].PROCID;
		vp = frametable[ii].VPAGE;

		counter[ii] >>= 1;
		if (pagetable[pid][vp].REFERENCED) {
			counter[ii] |= mask;
			pagetable[pid][vp].REFERENCED = 0;
		}
		if (counter[ii] < lowest_c) {
			lowest_c = counter[ii];
			lowest_p = ii;
		}
	}
	f = &frametable[lowest_p];
	return f;
}

void A_Pager::print_info() {
	unsigned int n = counter.size();
	cout << "A: ";
	for (unsigned int ii = 0; ii < n; ii++) {
		cout << counter[ii] << ' ';
	}
	cout << endl;
}