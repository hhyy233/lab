#ifndef __EVENT__
#define __EVENT__

#include "proc.h"

typedef enum { STATE_DONE, STATE_READY, STATE_RUN, STATE_BLOCK, STATE_CREATED } STATE;

class Event {
	int timestamp, timeInCurrState;
	Process * ptrPoc;
	STATE currState, nextState;

public:
	Event();
	Event(int ts, int tICS, Process* ptr, STATE cS, STATE nS);
	int get_timestamp();
	STATE get_currstate();
	STATE get_nextstate();
	int get_timeInCurrState();
	Process* get_proc();
};

#endif // !__EVENT__
