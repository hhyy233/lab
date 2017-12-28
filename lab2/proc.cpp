#include "proc.h"

Process::Process() {
	PID = 0;
	AT = TC = CB = IO = 0;
	PRIO = 0;
	FT = TT = IT = CW = 0;
	rem = MAX;
	ready_timestamp = cpu_burst = io_burst = 0;
	prio_dyn = PRIO-1;
}

Process::Process(int pid, int at, int tc, int cb, int io, int prio) {
	PID = pid;
	AT = at;
	TC = tc;
	CB = cb;
	IO = io;
	PRIO = prio;
	rem = TC;
	FT = TT = IT = CW = 0;
	ready_timestamp = cpu_burst = io_burst = 0;
	prio_dyn = PRIO-1;
}

int Process::get_pid() {
	return PID;
}

int Process::get_rem() {
	return rem;
}

int Process::get_AT() {
	return AT;
}

int Process::get_CB() {
	return CB;
}
int Process::get_IO() {
	return IO;
}

int Process::get_cpu_burst() {
	return cpu_burst;
}

int Process::get_io_burst() {
	return io_burst;
}

int Process::get_prio() {
	return PRIO;
}

int Process::get_prio_dyn() {
	return prio_dyn;
}

int Process::get_ready_timestamp() {
	return ready_timestamp;
}

void Process::get_Value(int& pid, int& at, int& tc, int& cb, int& io, int& prio, int& ft, int& tt, int& it, int& cw) {
	pid = PID;
	at = AT;
	tc = TC;
	cb = CB;
	io = IO;
	prio = PRIO;
	ft = FT;
	tt = TT;
	it = IT;
	cw = CW;
}

void Process::addIT(int vv) {
	IT += vv;
}

void Process::addCW(int vv) {
	CW += vv;
}
void Process::subTime(int vv) {
	cpu_burst -= vv;
	rem -= vv;
}

void Process::reset_prio_dyn() {
	prio_dyn = PRIO - 1;
}

void Process::sub_prio_dyn() {
	prio_dyn--;
}

void Process::set_rem(int rr) {
	rem = rr;
}

void Process::set_cpu_burst(int vv) {
	cpu_burst = vv;
}

void Process::set_io_burst(int vv) {
	io_burst = vv;
}

void Process::set_prio_dyn(int pp) {
	prio_dyn = pp;
}

void Process::set_value(int ts) {
	FT = ts;
	TT = FT - AT;
}

void Process::set_ready_timestamp(int current_time) {
	ready_timestamp = current_time;
}