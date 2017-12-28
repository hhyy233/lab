#pragma once

#include<iostream>
#include<list>
#include<vector>
#include<fstream>

#include "Bit.h"

using namespace std;

class Pager {
public:
	ifstream rfile;
	Pager();
	int myrand();
	virtual Frame* determine_victim_frame(vector<vector<PTE>>& pagetable, list<Frame*>& frame_list, vector<Frame>& frametable) = 0;
	virtual void print_info();//For aging
};

class FIFO_Pager :public Pager {
public:
	FIFO_Pager();
	Frame* determine_victim_frame(vector<vector<PTE>>& pagetable, list<Frame*>& frame_list, vector<Frame>& frametable);
};

class SC_Pager :public Pager {
public:
	SC_Pager();
	Frame* determine_victim_frame(vector<vector<PTE>>& pagetable, list<Frame*>& frame_list, vector<Frame>& frametable);
};

class R_Pager :public Pager {
public:
	R_Pager(string fn);
	Frame* determine_victim_frame(vector<vector<PTE>>& pagetable, list<Frame*>& frame_list, vector<Frame>& frametable);
};

class NRU_Pager :public Pager {
	int count;
public:
	NRU_Pager(string fn);
	Frame* determine_victim_frame(vector<vector<PTE>>& pagetable, list<Frame*>& frame_list, vector<Frame>& frametable);
};

class C_Pager :public Pager {
	int pointer;
public:
	C_Pager();
	Frame* determine_victim_frame(vector<vector<PTE>>& pagetable, list<Frame*>& frame_list, vector<Frame>& frametable);
};

class A_Pager :public Pager {
	vector<unsigned int> counter;
public:
	A_Pager();
	void print_info();
	Frame* determine_victim_frame(vector<vector<PTE>>& pagetable, list<Frame*>& frame_list, vector<Frame>& frametable);
};