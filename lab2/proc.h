#ifndef __PROCESS__
#define __PROCESS__

#include<iostream>

const int MAX = 10000;

class Process {
	int PID, AT, TC, CB, IO;
	//PID: process identification
	//AT: arrival time
	//TC: total runtime
	//CB: CPU burst
	//IO: I/O burst
	int PRIO, prio_dyn;
	//PRIO: priority 
	//prio_dyn: dynamic priority

	int FT, TT, IT, CW;
	//FT: finish time
	//TT: turnarround time
	//IT: I/O time, time in blocked state
	//CW: CPU waiting time

	int ready_timestamp,rem, cpu_burst, io_burst;
	//ready_timestamp record the time when it enter the ready queue
	//rem is remains time units

public:
	Process();
	Process(int pid, int at, int tc, int cb, int io, int prio);
	int get_pid();
	int get_rem();
	int get_AT();
	int get_CB();
	int get_IO();
	int get_cpu_burst();
	int get_io_burst();
	int get_prio();
	int get_prio_dyn();
	int get_ready_timestamp();
	void get_Value(int& pid, int& at, int& tc, int& cb, int& io, int& prio, int& ft, int& tt, int& it, int&cw);

	void addIT(int);
	void addCW(int);
	void subTime(int);//substract cpu_burst, rem
	void reset_prio_dyn();
	void sub_prio_dyn();
	void set_cpu_burst(int);
	void set_io_burst(int);
	void set_prio_dyn(int);
	void set_rem(int);
	void set_value(int);//FT,TT
	void set_ready_timestamp(int);
	
};

#endif // !__PROCESS__

