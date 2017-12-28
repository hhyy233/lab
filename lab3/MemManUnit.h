#pragma once

#include<iostream>
#include<fstream>
#include<list>
#include<vector>

#include "Bit.h"
#include "Pager.h"

using namespace std;

const int cost_map_un = 400;
const int cost_page_inout = 3000;
const int cost_file_inout = 2500;
const int cost_zero = 150;
const int cost_segv = 240;
const int cost_segprot = 300;
const int cost_readwrite = 1;
const int cost_cont_switch = 121;

struct proc_info {
	unsigned int pid;
	unsigned long int unmaps, maps;
	unsigned long int ins, outs;
	unsigned long int fins, fouts;
	unsigned long int zeros;
	unsigned long int segv, segprot;
	proc_info() {};
	proc_info(unsigned int pp) {
		pid = pp;
		unmaps = maps = 0;
		ins = outs = 0;
		fins = fouts = 0;
		zeros = 0;
		segv = segprot = 0;
	};
};

class MemManUnit {
	Pager* pager;
	list<Frame*> frame_list;
	vector<Frame> frametable;
	vector<vector<PTE>> pagetable;
	unsigned int curr_proc;
	unsigned long int ctx_switches, inst_count;
	vector<proc_info> pinf;
	unsigned long long int cost;
	bool O, P, F, S, f, a, x, y;
	bool isfree;
	void context_switch(int procid);
	Frame* get_frame();
	Frame* allocate_frame_from_free_list();
	void ref_page(char operation, int vpage);
	void print_pt(int procid);
	void print_ft();
	void print_sum();
public:
	MemManUnit();
	MemManUnit(char algo, string options, int num, string fn);
	void add_proc();
	void initialize_page(int sp, int ep, int wp, int fm);
	void process_inst(char opt, int opn);
	void print_res();
};