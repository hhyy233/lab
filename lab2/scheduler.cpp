#include "scheduler.h"

Scheduler::Scheduler() {
	quantum = MAX;
	run_end_ts = 0;
}

void Scheduler::putReady(Process* ptr, STATE ss) {
	readyqueue_ptr.push_back(ptr);
}

bool Scheduler::isRunning(int curent_time) {
	return curent_time < run_end_ts;
}

bool Scheduler::isEmpty() {
	return readyqueue_ptr.empty();
}

int Scheduler::get_quantum() {
	return quantum;
}

void Scheduler::set_run_end_ts(int end_time) {
	run_end_ts = end_time;
}

std::string Scheduler::get_typr() {
	switch (type) {
	case TYPE_F:return "FCFS";
	case TYPE_L:return "LCFS";
	case TYPE_S:return "SJF";
	case TYPE_P:return "PRIO";
	case TYPE_R:return "RR";
	}
}

Process* Scheduler::get_next_proc() {
	Process* ptrProc = readyqueue_ptr.front();
	readyqueue_ptr.pop_front();
	return ptrProc;
}

//FCFSScheduler
FCFSScheduler::FCFSScheduler() {
	quantum = MAX;
	run_end_ts = 0;
	type = TYPE_F;
}

//LCFSScheduler
LCFSScheduler::LCFSScheduler() {
	quantum = MAX;
	run_end_ts = 0;
	type = TYPE_L;
}

Process* LCFSScheduler::get_next_proc() {
	Process* ptrProc = readyqueue_ptr.back();
	readyqueue_ptr.pop_back();
	return ptrProc;
}

//SJFScheduler
SJFScheduler::SJFScheduler() {
	quantum = MAX;
	run_end_ts = 0;
	type = TYPE_S;
}

Process* SJFScheduler::get_next_proc() {
	Process* ptrProc = NULL;
	int short_time = MAX;

	std::list<Process*>::iterator it;
	std::list<Process*>::iterator it_short;
	for (it = readyqueue_ptr.begin(); it != readyqueue_ptr.end(); ++it) {
		int rem = (*it)->get_rem();
		if (rem < short_time) {
			short_time = rem;
			it_short = it;
			ptrProc = *it;
		}
	}
	readyqueue_ptr.erase(it_short);

	return ptrProc;
}

//RRScheduler
RRScheduler::RRScheduler(int qq) {
	quantum = qq;
	run_end_ts = 0;
	type = TYPE_R;
}

//PRIOScheduler
PRIOScheduler::PRIOScheduler(int qq) {
	quantum = qq;
	run_end_ts = 0;
	type = TYPE_P;

	for (int ii = 0; ii < 4; ii++) {
		std::list<Process*> la, lb;
		readyqueue_ptr_active.push_back(la);
		readyqueue_ptr_idle.push_back(lb);
	}
}

Process* PRIOScheduler::get_next_proc() {
	Process* ptrProc = NULL;
	//
	int ii = 0;
	bool flag = true;
	while (flag && ii<4) {
		flag = flag && readyqueue_ptr_active[ii].empty();
		ii++;
	}
	if (flag) {
		readyqueue_ptr_active = readyqueue_ptr_idle;
		for (int ii = 0; ii < 4; ii++) {
			readyqueue_ptr_idle[ii].clear();
		}
	}

	ii = 3; //process with high priority pop first
	while (readyqueue_ptr_active[ii].empty()) ii--;
	ptrProc = readyqueue_ptr_active[ii].front();
	readyqueue_ptr_active[ii].pop_front();

	return ptrProc;
}

bool PRIOScheduler::isEmpty() {
	bool flag = true;
	int ii = 0;
	while (flag && ii<4) {
		flag = flag && readyqueue_ptr_active[ii].empty() && readyqueue_ptr_idle[ii].empty();
		ii++;
	}
	return flag;
}

void PRIOScheduler::putReady(Process* ptr, STATE ss) {
	int at = ptr->get_AT();
	int rem = ptr->get_rem();
	int p_d = -1;
	if (ss == STATE_CREATED || ss == STATE_BLOCK)
		ptr->reset_prio_dyn();
	else
		ptr->sub_prio_dyn();
	int prio_dyn = ptr->get_prio_dyn();

	if (prio_dyn != -1) {
		//process just created
		readyqueue_ptr_active[prio_dyn].push_back(ptr);
	}
	else {
		ptr->reset_prio_dyn();
		prio_dyn = ptr->get_prio_dyn();
		readyqueue_ptr_idle[prio_dyn].push_back(ptr);
	}
	//readyqueue_ptr_idle[prio_dyn].push_back(ptr);
}