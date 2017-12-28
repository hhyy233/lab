#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include<string>
#include<unistd.h>

#include "MemManUnit.h"
#include "Bit.h"

using namespace std;

ifstream infile;
ifstream rfile;
MemManUnit mmu;

bool get_next_line(string& buffer) {
	while (!infile.eof()) {
		getline(infile, buffer);
		if (buffer.size()>0&&buffer[0] != '#') return true;
	}
	return false;
}

void simulate() {
	string buffer;
	
	get_next_line(buffer);
	int num;
	stringstream(buffer) >> num;
	for (int ii = 0; ii < num; ii++) {
		get_next_line(buffer);
		int n1;
		stringstream(buffer) >> n1;
		mmu.add_proc();
		for (int jj = 0; jj < n1; jj++) {
			int sp, ep, wp, fm;
			get_next_line(buffer);
			stringstream(buffer) >> sp >> ep >> wp >> fm;
			//do something
			//cout << sp << ep << wp << fm << endl;//
			mmu.initialize_page(sp, ep, wp, fm);
		}
	}
	char opt;
	int opn;
	while (get_next_line(buffer)) {
		stringstream(buffer) >> opt >> opn;
		//cout << opt << opn << endl;//
		mmu.process_inst(opt, opn);
		//system("pause");
	}
	mmu.print_res();
}

int main(int argc, char* argv[]) {
	char algo;
	string options;
	int num_frames;
	string filename, rfilename;
	
	string value;
	int c;
	while ((c = getopt(argc, argv, "a:o:f:")) != -1)
		switch (c)
		{
		case 'a':
			algo = optarg[0];
			break;
		case 'o':
			options = optarg;
			break;
		case 'f':
			value = optarg;
			break;
		}
	num_frames = stoi(value);
	filename = argv[argc - 2];
	rfilename = argv[argc - 1];
	//
	// cout << algo << ' ' << options << ' ' << value << ' ' << filename << ' ' << rfilename << endl;
	
	infile.open(filename);
	rfile.open(rfilename);
	if (!infile || !rfile) {
		cout << "Can't not open file!!!" << endl;
		return 1;
	}
	//parse comment
	rfile.close();

	mmu = MemManUnit(algo, options, num_frames, rfilename);

	simulate();

	infile.close();

	return 0;
}