#ifndef __IOUTILITY__
#define __IOUTILITY__

class IOUtility {
	int total_time;
	int io_begin;
	int io_end;

public:
	IOUtility();
	void addtime(int, int);
	int get_time();
};

#endif // !__IOUTILITY__
