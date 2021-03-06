CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

Sim05 : Sim05.o data.o process.o 
	$(CC) $(LFLAGS) Sim05.o data.o process.o -pthread -o Sim05

Sim05.o : Sim05.cpp data.cpp process.cpp readData.cpp simFuncs.cpp 
	$(CC) $(CFLAGS) Sim05.cpp 

data.o : data.cpp data.h
	$(CC) $(CFLAGS) data.cpp

process.o: process.cpp process.h
	$(CC) $(CFLAGS) process.cpp

clean: 
	\rm *.o Sim05
