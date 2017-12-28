#include "ioschedular.h"

ioschedular::ioschedular() {
	tot_movement = 0;
	verbose = false;
	q_v = false;
	f_v = false;
	busy = false;
	curtrack = 0;
	dir = 1;
}

void ioschedular::set_v() {
	verbose = true;
	cout << "TRACE" << endl;
}

void ioschedular::set_q() {
	q_v = true;
}

void ioschedular::set_f() {
	f_v = true;
}

bool ioschedular::is_busy() {
	return  busy;
}

void ioschedular::addio(int timestep, io* io_req) {
	add_list(io_req);
	//verbose
	if (verbose)
		printf("%d:%6d add %d\n", timestep, io_req->ioop, io_req->track);
	if (f_v) show_info();
}

int ioschedular::get_tot_movement() {
	return tot_movement;
}

void ioschedular::moveon(int timestep) {

	bool flag = true;
	while (flag) {
		flag = false;
		//complete
		if (busy && curtrack == (*it)->track) {
			(*it)->end_time = timestep;
			//verbose
			if (verbose) {
				printf("%d:%6d finish %d\n", timestep, (*it)->ioop, (*it)->end_time - (*it)->arrival_time);
			}
			removeio();
			busy = false;
		}
		//issue
		if (!busy) {
			if (get_next_io()) {
				if (q_v) show_queue(timestep);
				(*it)->start_time = timestep;
				busy = true;
				//verbose
				if (verbose)
					printf("%d:%6d issue %d %d\n", timestep, (*it)->ioop, (*it)->track, curtrack);

				//in case
				if (curtrack == (*it)->track) flag = true;
			}
		}
	}

	if (busy) {
		curtrack += dir;
		tot_movement++;
	}
}

void ioschedular::add_list(io* io_req) {
	waiting_list.push_back(io_req);
}

void ioschedular::removeio() {
	waiting_list.erase(it);
}

void ioschedular::show_queue(int timestep) {

}

void ioschedular::show_info() {

}


//FIFO
FIFO_ioschedular::FIFO_ioschedular() {
	tot_movement = 0;
	verbose = false;
	q_v = false;
	f_v = false;
	busy = false;
	curtrack = 0;
	dir = 0;
}

bool FIFO_ioschedular::get_next_io() {
	if (waiting_list.empty()) return false;
	it = waiting_list.begin();

	if ((*it)->track > curtrack) dir = 1;
	else dir = -1;
	return true;
}


//SSTF
SSTF_ioschedular::SSTF_ioschedular() {
	tot_movement = 0;
	verbose = false;
	q_v = false;
	f_v = false;
	busy = false;
	curtrack = 0;
	dir = 0;
}

//void SSTF_ioschedular::add_list(io* io_req) {
//	if (waiting_list.empty()) {
//		waiting_list.push_back(io_req);
//		return;
//	}
//	io* pointer = waiting_list.back();
//	if (io_req->track > pointer->track) {
//		waiting_list.push_back(io_req);
//		return;
//	}
//	pointer = waiting_list.front();
//	if (io_req->track < pointer->track) {
//		waiting_list.push_front(io_req);
//		return;
//	}
//	list<io*>::iterator p;
//	p = waiting_list.begin();
//	while (io_req->track > (*p)->track) ++p;
//	waiting_list.insert(p, io_req);
//}

bool SSTF_ioschedular::get_next_io() {
	if (waiting_list.empty()) return false;

	//if (curtrack == 0) {
	//	it = waiting_list.begin();
	//}
	////else if (it == waiting_list.end()) {
	////	it--;
	////	//it = prev(it);
	////}
	//else {
	//	it--;
	//	list<io*>::iterator p = it;
	//	p++;
	//	if (p != waiting_list.end()) {
	//		int d1 = curtrack - (*p)->track;
	//		int d2 = curtrack - (*it)->track;
	//		if (d1 < d2) it = p;
	//	}
	//}

	list<io*>::iterator p;
	int sm = INT_MAX;
	int diff;
	for (p = waiting_list.begin(); p != waiting_list.end(); ++p) {
		diff = abs(curtrack - (*p)->track);
		if (diff < sm) {
			sm = diff;
			it = p;
		}
	}

	if ((*it)->track > curtrack) dir = 1;
	else dir = -1;
	return true;
}

void SSTF_ioschedular::show_queue(int timestep) {
	list<io*>::iterator p;
	cout << "    ";
	for (p = waiting_list.begin(); p != waiting_list.end(); ++p) {
		printf("%d:%d ", (*p)->ioop, abs((*p)->track - curtrack));
	}
	cout << endl;
}


//LOOK
LOOK_ioschedular::LOOK_ioschedular() {
	tot_movement = 0;
	verbose = false;
	q_v = false;
	f_v = false;
	busy = false;
	change_dir = false;
	curtrack = 0;
	dir = 1;
}

bool LOOK_ioschedular::get_next_io() {
	change_dir = false;
	if (waiting_list.empty()) return false;
	/*else if (curtrack == 0) {
		it = waiting_list.begin();
		return true;
	}*/

	bool flag = false;
	list<io*>::iterator p;
	int diff;
	int sm = INT_MAX;
	//
	while (!flag) {
		if (dir > 0) {//up
			for (p = waiting_list.begin(); p != waiting_list.end(); ++p) {
				diff = (*p)->track - curtrack;
				if (diff >= 0 && diff < sm) {
					sm = diff;
					it = p;
					flag = true;
				}
			}
		}
		else {//down
			for (p = waiting_list.begin(); p != waiting_list.end(); ++p) {
				diff = curtrack - (*p)->track;
				if (diff >= 0 && diff < sm) {
					sm = diff;
					it = p;
					flag = true;
				}
			}
		}
		if (!flag) {
			dir = -dir;
			change_dir = true;
		}
	}

	return true;
}

void LOOK_ioschedular::show_queue(int timestep) {
	list<io*>::iterator p;
	int diff;
	if (change_dir) {
		printf("    Get: () --> change direction to %d\n", dir);
	}
	cout << "    Get: (";
	for (p = waiting_list.begin(); p != waiting_list.end(); ++p) {
		diff = (*p)->track - curtrack;
		if ((dir > 0 && diff >= 0) || (dir < 0 && diff <= 0))
			printf("%d:%d ", (*p)->ioop, abs(diff));
	}
	printf(") --> %d dir=%d\n", (*it)->ioop, dir);
}


//CLOOK
CLOOK_ioschedular::CLOOK_ioschedular() {
	tot_movement = 0;
	verbose = false;
	q_v = false;
	f_v = false;
	busy = false;
	wrap = false;
	curtrack = 0;
	dir = 1;
}

bool CLOOK_ioschedular::get_next_io() {
	dir = 1;
	wrap = false;
	if (waiting_list.empty()) return false;

	bool flag = false;
	list<io*>::iterator p;
	int diff;
	int sm = INT_MAX;
	int small = INT_MAX;
	list<io*>::iterator sp;
	//
	for (p = waiting_list.begin(); p != waiting_list.end(); ++p) {
		diff = (*p)->track - curtrack;
		if (diff >= 0 && diff < sm) {
			sm = diff;
			it = p;
			flag = true;
		}
		if ((*p)->track < small) {
			small = (*p)->track;
			sp = p;
		}
	}

	if (!flag) {
		it = sp;
		dir = -1;
		wrap = true;
	}

	return true;
}

void CLOOK_ioschedular::show_queue(int timestep) {
	if (wrap) {
		printf("    Get: () --> go to bottom and pick %d\n", (*it)->ioop);
		return;
	}
	list<io*>::iterator p;
	int diff;
	cout << "    Get: (";
	for (p = waiting_list.begin(); p != waiting_list.end(); ++p) {
		diff = (*p)->track - curtrack;
		if (diff >= 0) printf("%d:%d ", (*p)->ioop, diff);
	}
	printf(") --> %d\n", (*it)->ioop);
}


//FLOOK
FLOOK_ioschedular::FLOOK_ioschedular() {
	tot_movement = 0;
	verbose = false;
	q_v = false;
	f_v = false;
	busy = false;
	change_dir = false;
	change_list = false;
	curtrack = 0;
	dir = 1;
	waiting_list = vector<list<io*>>(2, list<io*>());
	active = 1;
}

void FLOOK_ioschedular::add_list(io* io_req) {
	int index = (active + 1) % 2;
	waiting_list[index].push_back(io_req);
}

bool FLOOK_ioschedular::get_next_io() {
	change_dir = false;
	change_list = false;
	if (waiting_list[0].empty() && waiting_list[1].empty()) return false;

	//
	if (waiting_list[active].empty()) {
		active = (active + 1) % 2;
		change_list = true;
	}

	bool flag = false;
	list<io*>::iterator p;
	int diff;
	int sm = INT_MAX;
	//
	while (!flag) {
		if (dir > 0) {//up
			for (p = waiting_list[active].begin(); p != waiting_list[active].end(); ++p) {
				diff = (*p)->track - curtrack;
				if (diff >= 0 && diff < sm) {
					sm = diff;
					it = p;
					flag = true;
				}
			}
		}
		else {//down
			for (p = waiting_list[active].begin(); p != waiting_list[active].end(); ++p) {
				diff = curtrack - (*p)->track;
				if (diff >= 0 && diff < sm) {
					sm = diff;
					it = p;
					flag = true;
				}
			}
		}
		if (!flag) {
			dir = -dir;
			change_dir = true;
		}
	}

	return true;
}

void FLOOK_ioschedular::removeio() {
	waiting_list[active].erase(it);
}

void FLOOK_ioschedular::show_queue(int timestep) {
	list<io*>::iterator p;
	printf("AQ=%d dir=%d curtrack=%d:  ", active, change_dir ? -dir : dir, curtrack);
	printf("Q[0] = ( ");
	for (p = waiting_list[0].begin(); p != waiting_list[0].end(); ++p) {
		printf("%d:%d:%d ", (*p)->ioop, (*p)->track, (*p)->track - curtrack);
	}
	printf(")  Q[1] = ( ");
	for (p = waiting_list[1].begin(); p != waiting_list[1].end(); ++p) {
		printf("%d:%d:%d ", (*p)->ioop, (*p)->track, (*p)->track - curtrack);
	}
	printf(")\n");

	if (change_dir) {
		printf("    Get: () --> change direction to %d\n", dir);
	}

	int diff;
	cout << "    Get: (";
	for (p = waiting_list[active].begin(); p != waiting_list[active].end(); ++p) {
		diff = (*p)->track - curtrack;
		if ((dir > 0 && diff >= 0) || (dir < 0 && diff <= 0))
			printf("%d:%d:%d ", (*p)->ioop, (*p)->track, abs(diff));
	}
	printf(") --> %d dir=%d\n", (*it)->ioop, dir);

	printf("%d:%8d get Q=%d\n", timestep, (*it)->ioop, active);
}

void FLOOK_ioschedular::show_info() {
	int index = (active + 1) % 2;
	list<io*>::iterator p;
	printf("   Q=%d ( ", index);
	for (p = waiting_list[index].begin(); p != waiting_list[index].end(); ++p) {
		printf("%d:%d ", (*p)->ioop, (*p)->track);
	}
	printf(")\n");
}