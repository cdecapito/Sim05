CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

Sim04 : Sim04.o data.o process.o 
	$(CC) $(LFLAGS) Sim04.o data.o process.o -pthread -o Sim04

Sim04.o : Sim04.cpp data.cpp process.cpp readData.cpp simFuncs.cpp 
	$(CC) $(CFLAGS) Sim04.cpp 

data.o : data.cpp data.h
	$(CC) $(CFLAGS) data.cpp

process.o: process.cpp process.h
	$(CC) $(CFLAGS) process.cpp

clean: 
	\rm *.o Sim04