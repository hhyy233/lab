CC = g++
CFLAGS  = -g -Wall
# CFLAGS  = -Wall

# default: mmu

sched:  hw3.o MemManUnit.o Pager.o 
	$(CC) $(CFLAGS) -o mmu hw3.o MemManUnit.o Pager.o 

hw3.o:  hw3.cpp MemManUnit.h Pager.h Bit.h 
	$(CC) $(CFLAGS) -c hw3.cpp

MemManUnit.o:  MemManUnit.cpp MemManUnit.h 
	$(CC) $(CFLAGS) -c MemManUnit.cpp

Pager.o:  Pager.cpp Pager.h 
	$(CC) $(CFLAGS) -c Pager.cpp
	

clean: 
	rm -rf *.o *~
	# rm -f mmu *~