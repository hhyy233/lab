#include "event.h"

Event::Event() {
	timestamp = 0;
	timeInCurrState = 0;
}

Event::Event(int ts, int tICS, Process* ptr, STATE cS, STATE nS) {
	timestamp = ts;
	timeInCurrState = tICS;
	ptrPoc = ptr;
	currState = cS;
	nextState = nS;
}

int Event::get_timestamp() {
	return timestamp;
}

STATE Event::get_currstate() {
	return currState;
}

STATE Event::get_nextstate() {
	return nextState;
}

int Event::get_timeInCurrState() {
	return timeInCurrState;
}

Process* Event::get_proc() {
	return ptrPoc;
}