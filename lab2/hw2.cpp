#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>

#include "proc.h"
#include "scheduler.h"
#include "event.h"
#include "ioutility.h"

using namespace std;

ifstream infile;
ifstream rfile;
bool verbose = false;

vector<Process> processes;
list<Event*> evt_queue;
Scheduler* sched;
IOUtility iou;

int myrand(int burst) {
	int randval;
	rfile >> randval;
	if (randval == EOF) {
		rfile.seekg(0, ios::beg);
		rfile >> randval;
		rfile >> randval;
	}
	return 1 + randval%burst;
}

void loadFile() {
	int pid = 0;
	int at, tc, cb, io, prio;
	Event* ptrEvt = NULL;
	while (infile>>at) {
		infile >> tc >> cb >> io;
		prio = myrand(4);
		Process proc(pid, at, tc, cb, io, prio);
		processes.push_back(proc);
		
		pid++;
	}
	for (int ii = 0; ii < processes.size(); ii++) {
		ptrEvt = new Event(processes[ii].get_AT(), 0, &processes[ii], STATE_CREATED, STATE_READY);
		evt_queue.push_back(ptrEvt);
	}
}

string show_state(STATE ss) {
	switch (ss) {
	case STATE_CREATED: return "CREATED";
	case STATE_RUN: return "RUNNG";
	case STATE_BLOCK: return "BLOCK";
	case STATE_READY: return "READY";
	case STATE_DONE: return "Done";
	}
}

int get_next_event_time() {
	if (evt_queue.empty()) return MAX; //error check
	return evt_queue.front()->get_timestamp();
}

void printInfo(Event evt) {
	if (verbose) {
		int ts = evt.get_timestamp();
		int tics = evt.get_timeInCurrState();
		Process* ptr = evt.get_proc();
		int pid = ptr->get_pid();
		STATE currstate = evt.get_currstate();
		STATE nextstate = evt.get_nextstate();
		if (nextstate == STATE_BLOCK) {
			printf("%d %d %d: RUNNG -> BLOCK  ib=%d rem=%d\n", ts, pid, tics, ptr->get_io_burst(), ptr->get_rem());
		}
		else if (currstate == STATE_BLOCK && nextstate == STATE_READY) {
			printf("%d %d %d: BLOCK -> READY\n", ts, pid, tics);
		}
		else if (currstate == STATE_RUN && nextstate == STATE_READY) {
			printf("%d %d %d: RUNNG -> READY  cb=%d rem=%d prio=%d\n", ts, pid, tics, ptr->get_cpu_burst(), ptr->get_rem(), ptr->get_prio_dyn());
		}
		else if (nextstate == STATE_READY) {
			printf("%d %d %d: CREATED -> READY\n", ts, pid, tics);
		}
		else if (nextstate == STATE_RUN) {
			printf("%d %d %d: READY -> RUNNG cb=%d rem=%d prio=%d\n", ts, pid, tics, ptr->get_cpu_burst(), ptr->get_rem(), ptr->get_prio_dyn());
		}
		else {
			printf("%d %d %d: Done\n", ts, pid, tics);
		}
	}
	//cin.get();
}

void put_event(Event* evt) {
	int ts_c = evt->get_timestamp();
	int ts_s = 0;
	if (evt_queue.empty()) {
		evt_queue.push_back(evt);
		return;
	}
	Process* ptr = evt->get_proc();

	list<Event*>::iterator it;
	for (it = evt_queue.begin(); it != evt_queue.end(); it++) {
		ts_s = (*it)->get_timestamp();
		if (ts_c < ts_s) break;
	}
	evt_queue.insert(it, evt);
}

void run_proc(int current_time) {
	//get the process to be run
	Process* ptrProc = sched->get_next_proc();
	int ready_waittime = current_time - ptrProc->get_ready_timestamp();
	ptrProc->addCW(ready_waittime);

	//create run process
	int cb = ptrProc->get_cpu_burst();
	int rem = ptrProc->get_rem();
	int quantum = sched->get_quantum();

	int run_time;
	STATE oldState = STATE_READY;
	STATE nextState;
	if (cb > quantum) {
		//come from preempt
		run_time = quantum;
		nextState = STATE_READY;
	}
	else if (cb == rem) {
		//go to done
		run_time = cb;
		nextState = STATE_DONE;
	}
	else if (cb == 0) {
		//come from block
		oldState = STATE_BLOCK;
		cb = myrand(ptrProc->get_CB());
		if (cb > rem) cb = rem;
		ptrProc->set_cpu_burst(cb);
		if (cb > quantum) {
			run_time = quantum;
			nextState = STATE_READY;
		}
		else if (cb == rem) {
			run_time = cb;
			nextState = STATE_DONE;
		}
		else {
			run_time = cb;
			nextState = STATE_BLOCK;
		}
	}
	else {
		//go to block
		run_time = cb;
		nextState = STATE_BLOCK;
	}
	printInfo(Event(current_time, ready_waittime, ptrProc, oldState, STATE_RUN));
	//cin.get();
	ptrProc->subTime(run_time);
	Event* ptrEvt_run = new Event(current_time + run_time, run_time, ptrProc, STATE_RUN, nextState);
	sched->set_run_end_ts(current_time + run_time);
	put_event(ptrEvt_run);
}

void simulation() {
	int current_time = 0;
	while (!evt_queue.empty()) {
		Event* evt = evt_queue.front();
		evt_queue.pop_front();
		Process* pp = evt->get_proc();
		current_time = evt->get_timestamp();

		//verbose
		//printInfo(*evt);

		switch (evt->get_nextstate())
		{
		case STATE_DONE:
			printInfo(*evt);//verbose
			pp->set_value(current_time);
			break;
		case STATE_READY:
			printInfo(*evt);//verbose
			pp->set_ready_timestamp(current_time);
			sched->putReady(pp, evt->get_currstate());
			break;
		case STATE_BLOCK:
			int bl_time = myrand(pp->get_IO());
			pp->set_io_burst(bl_time);
			pp->addIT(bl_time);
			printInfo(*evt);//verbose
			Event* ptrEvt_bl = new Event(current_time + bl_time, bl_time, pp, STATE_BLOCK, STATE_READY);
			iou.addtime(current_time, current_time + bl_time);
			put_event(ptrEvt_bl);
			break;
		}
		delete evt;
		//cin.get();

		//run scheduler;
		if (get_next_event_time() == current_time || sched->isRunning(current_time) || sched->isEmpty())
			continue;
		run_proc(current_time);
	}

}

void printRes() {
	int pid, at, tc, cb, io, prio, ft, tt, it, cw;
	int ftole = 0;
	double total_cputime, total_turnaround, total_waittime;
	total_cputime = total_turnaround = total_waittime = 0.0;
	string type = sched->get_typr();
	cout << type;
	if (type == "PRIO" || type == "RR") cout << ' ' << sched->get_quantum();
	cout << endl;
	int num_proc = processes.size();
	for (int ii = 0; ii < num_proc; ii++) {
		processes[ii].get_Value(pid, at, tc, cb, io, prio, ft, tt, it, cw);
		printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n", pid, at, tc, cb, io, prio, ft, tt, it, cw);
		ftole = ftole > ft ? ftole : ft;
		total_cputime += tc;
		total_turnaround += tt;
		total_waittime += cw;
	}
	int iotime = iou.get_time();
	//cout << iotime << endl;
	printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", ftole, total_cputime*100 / ftole, double(iotime *100) / ftole, total_turnaround / num_proc, total_waittime / num_proc, double(num_proc * 100) / ftole);
}

int main(int argc, char* argv[]) {

	// string filename = "C:/Users/HH/Desktop/lab2_assign/lab2_assign/input6";
	// string rfilename = "C:/Users/HH/Desktop/lab2_assign/lab2_assign/rfile";
	// infile.open(filename.c_str());
	// rfile.open(rfilename.c_str());
	// verbose = true;
	// char type = 'P';//F,L,P,R,S
	// int qq = 5;//quantum

	if (argc < 4) {
		cout << "Invalid parameter!!" << endl;
		return 1;
	}
	int qq = MAX;
	char type;
	bool flag = true;
	for (int ii = 1; ii < argc; ii++) {
		if (argv[ii][0] == '-' && argv[ii][1] == 'v') {
			verbose = true;
		}
		else if (argv[ii][0] == '-') {
			type = argv[ii][2];
			if (type == 'P' || type == 'R') {
				sscanf(argv[ii] + 3, "%d", &qq);
			}
		}
		else {
			if (flag){
				infile.open(argv[ii]);
				flag = false;
			}
			else
				rfile.open(argv[ii]);
		}
	}

	if (!infile || !rfile) {
		cout << "Can't open file!!" << endl;
		return 2;
	}

	int randval;
	rfile >> randval;
	loadFile();
	infile.close();

	switch (type)
	{
	case 'F':sched = new FCFSScheduler(); break;
	case 'L':sched = new LCFSScheduler(); break;
	case 'S':sched = new SJFScheduler(); break;
	case 'P':sched = new PRIOScheduler(qq); break;
	case 'R':sched = new RRScheduler(qq); break;
	}

	simulation();
	printRes();

	rfile.close();

	return 0;
}