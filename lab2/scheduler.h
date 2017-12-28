#ifndef __SCHEDULER__
#define __SCHEDULER__

#include <list>
#include <vector>
#include "proc.h"
#include "event.h"

typedef enum {TYPE_F, TYPE_L, TYPE_S, TYPE_R, TYPE_P} SCHEDUL_TYPE;

class Scheduler {
protected:
	SCHEDUL_TYPE type;
	int quantum;
	int run_end_ts;//record the end time of the running process 
	std::list<Process*> readyqueue_ptr;

public:
	Scheduler();
	bool isRunning(int);
	int get_quantum();
	void set_run_end_ts(int);
	std::string get_typr();

	virtual bool isEmpty();
	virtual void putReady(Process*, STATE);
	virtual Process* get_next_proc();
};

//FCFSScheduler
class FCFSScheduler : public Scheduler {
	
public:
	FCFSScheduler();
};

//LCFSScheduler
class LCFSScheduler : public Scheduler {

public:
	LCFSScheduler();
	Process* get_next_proc();
};

//SJFScheduler
class SJFScheduler :public Scheduler {

public:
	SJFScheduler();
	Process* get_next_proc();
};

//RRScheduler
class RRScheduler :public Scheduler {

public:
	RRScheduler(int);
};


//PRIOScheduler
class PRIOScheduler :public Scheduler {
	std::vector< std::list<Process*> > readyqueue_ptr_active;
	std::vector< std::list<Process*> > readyqueue_ptr_idle;
public:
	PRIOScheduler(int);
	Process* get_next_proc();
	bool isEmpty();
	void putReady(Process*, STATE);
};

#endif // !__SCHEDULER__