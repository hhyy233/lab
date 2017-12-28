CC = g++
CFLAGS  = -g -Wall
# CFLAGS  = -Wall

# default: iosched

iosched:  hw4.o ioschedular.o
	$(CC) $(CFLAGS) -o iosched hw4.o ioschedular.o

hw4.o:  hw4.cpp ioschedular.h
	$(CC) $(CFLAGS) -c hw4.cpp

ioschedular.o:  ioschedular.cpp ioschedular.h 
	$(CC) $(CFLAGS) -c ioschedular.cpp

clean: 
	rm -rf *.o *~
	# rm -f iosched *~