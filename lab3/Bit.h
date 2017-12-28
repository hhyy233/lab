#pragma once
struct PTE{ //page table entry
	unsigned int PRESENT : 1;
	unsigned int WRITE_PROTECT : 1;
	unsigned int MODIFIED : 1;
	unsigned int REFERENCED : 1;
	unsigned int PAGEOUT : 1;
	//
	unsigned int FILE_MAPPED : 1;
	unsigned int ACCESSABLE : 1;
	unsigned int IDLE : 18;
	//frame
	unsigned int FRAMEID : 7;
	PTE() {
		PRESENT = 0;
		WRITE_PROTECT = 0;
		MODIFIED = 0;
		REFERENCED = 0;
		PAGEOUT = 0;
		FILE_MAPPED = 0;
		ACCESSABLE = 0;
		IDLE = 0;
		FRAMEID = 0;
	}
	void setbit(int wp, int fm) {
		WRITE_PROTECT = wp;
		FILE_MAPPED = fm;
		ACCESSABLE = 1;
	}
};//32 bits

struct Frame {//frame table tag
	unsigned int FRAMEID : 7;
	unsigned int PROCID : 19;
	unsigned int VPAGE : 6;
	Frame() {
		FRAMEID = 0;
		PROCID = 0;
		VPAGE = 0;
	};
};//32 bits

//test size
//cout << sizeof(PTE) << endl;
//cout << sizeof(TAG) << endl;