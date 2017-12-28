#pragma once
#include<list>
#include<iostream>
#include<vector>
#include<climits>
#include<stdlib.h>

using namespace std;

struct io {
	int ioop;//IO-op#
	int arrival_time;
	int track;
	int start_time;
	int end_time;
	io(int i, int at, int t) :ioop(i), arrival_time(at), track(t), start_time(-1), end_time(-1) {};
};

class ioschedular {
public:
	int tot_movement;
	bool verbose;
	bool q_v;
	bool f_v;//for FLOOK
	bool busy; 
	list<io*> waiting_list;
	list<io*>::iterator it;
	int curtrack;
	int dir;

	ioschedular();
	void set_v();
	void set_q();
	void set_f();
	bool is_busy();
	void addio(int, io*);
	int get_tot_movement();
	void moveon(int);

	virtual void show_queue(int);
	virtual void show_info();
	virtual void add_list(io*);
	virtual void removeio();

	virtual bool get_next_io() = 0;
};

class FIFO_ioschedular :public ioschedular {
	bool get_next_io();
public:
	FIFO_ioschedular();
};

class SSTF_ioschedular :public ioschedular {
	bool get_next_io();
	void show_queue(int);
public:
	SSTF_ioschedular();
};

class LOOK_ioschedular :public ioschedular {
	bool change_dir;

	bool get_next_io();
	void show_queue(int);
public:
	LOOK_ioschedular();
};

class CLOOK_ioschedular :public ioschedular {
	bool wrap;

	bool get_next_io();
	void show_queue(int);
public:
	CLOOK_ioschedular();
};

class FLOOK_ioschedular :public ioschedular {
	vector<list<io*>> waiting_list;
	int active;
	bool change_dir;
	bool change_list;

	//void moveon(int);
	void add_list(io*);
	bool get_next_io();
	void removeio();
	void show_queue(int);
	void show_info();
public:
	FLOOK_ioschedular();
};