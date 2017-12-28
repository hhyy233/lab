#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<list>
#include<vector>
#include<unistd.h>

#include "ioschedular.h"

using namespace std;

ifstream infile;
vector<io> io_request;
ioschedular* iosched;

int curr_time = 0;

bool get_next_line(string& buffer) {
	while (!infile.eof()) {
		getline(infile, buffer);
		if (buffer.size() > 0 && buffer[0] != '#') return true;
	}
	return false;
}

void parse_comment() {
	string buffer;
	int timestep, track;
	while (get_next_line(buffer)) {
		stringstream(buffer) >> timestep >> track;
		io_request.push_back(io(io_request.size(), timestep, track));
	}
}

void print_result() {
	double tot_turnaround = 0;
	double tot_waittime = 0;
	int max_waittime = 0;

	for (int ii = 0; ii < io_request.size(); ii++) {
		io oo = io_request[ii];
		printf("%5d: %5d %5d %5d\n", ii, oo.arrival_time, oo.start_time, oo.end_time);
		int ta = oo.end_time - oo.arrival_time;
		tot_turnaround += ta;
		int wt = oo.start_time - oo.arrival_time;
		tot_waittime += wt;
		if (wt > max_waittime) max_waittime = wt;
	}
	printf("SUM: %d %d %.2lf %.2lf %d\n",
		curr_time, iosched->get_tot_movement(), tot_turnaround / io_request.size(), tot_waittime / io_request.size(), max_waittime);
}

void simulate() {
	int next_ioreq = 0;
	while (next_ioreq < io_request.size() || iosched->is_busy()) {
		curr_time++;
		//arrival
		if (next_ioreq < io_request.size() && curr_time == io_request[next_ioreq].arrival_time) {
			iosched->addio(curr_time, &io_request[next_ioreq]);
			next_ioreq++;
		}
		//completion
		//issue
		iosched->moveon(curr_time);
	}
	print_result();
}


int main(int argc, char* argv[]) {
	string filename;
	char algo;
	bool verbose = false;
	bool q_req = false;
	bool f_req = false;

	int c;
	while ((c = getopt(argc, argv, "s:vqf")) != -1)
		switch (c)
		{
		case 's':
			algo = optarg[0];
			break;
		case 'v':
			verbose = true;
			break;
		case 'q':
			q_req = true;
			break;
		case 'f':
			f_req = true;
			break;
		}
	filename = argv[argc - 1];

	switch (algo)
	{
	case 'i': iosched = new FIFO_ioschedular(); break;
	case 'j': iosched = new SSTF_ioschedular(); break;
	case 's': iosched = new LOOK_ioschedular(); break;
	case 'c': iosched = new CLOOK_ioschedular(); break;
	case 'f': iosched = new FLOOK_ioschedular(); break;
	}

	if (verbose) iosched->set_v();
	if (q_req) iosched->set_q();
	if (f_req) iosched->set_f();

	infile.open(filename);
	parse_comment();
	simulate();

	infile.close();

	return 0;
}