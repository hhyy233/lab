CC = g++
# CFLAGS  = -g -Wall
CFLAGS  = -Wall

# default: sched

sched:  hw2.o event.o proc.o scheduler.o ioutility.o
	$(CC) $(CFLAGS) -o sched hw2.o event.o proc.o scheduler.o ioutility.o

hw2.o:  hw2.cpp event.h proc.h scheduler.h ioutility.h
	$(CC) $(CFLAGS) -c hw2.cpp

event.o:  event.cpp event.h 
	$(CC) $(CFLAGS) -c event.cpp

proc.o:  proc.cpp proc.h 
	$(CC) $(CFLAGS) -c proc.cpp
	
scheduler.o:  scheduler.cpp scheduler.h 
	$(CC) $(CFLAGS) -c scheduler.cpp

ioutility.o:  ioutility.cpp ioutility.h 
	$(CC) $(CFLAGS) -c ioutility.cpp

clean: 
	rm -rf *.o *~
	# rm -f sched *~