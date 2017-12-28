#include "ioutility.h"

IOUtility::IOUtility() {
	total_time = io_begin = io_end = 0;
}

void IOUtility::addtime(int t_b, int t_e) {
	if (t_b < io_end) {
		if (t_e > io_end) {
			total_time += t_e - io_end;
			io_end = t_e;
		}
	}
	else {
		io_begin = t_b;
		io_end = t_e;
		total_time += (io_end - io_begin);
	}
}

int IOUtility::get_time() {
	return total_time;
}