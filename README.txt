CS 446/646 Project 4: Sim04

Written by: Carli DeCapito 
		Version 1.11 CPU Scheduling Algorithms ( 11/14/17 )
		Version 1.10 Multiprocessing/Concurrency (10/22/17)
	    Version 1.01 Threading & Processing (10/8/17)
	    Version 1.00 Original (9/18/17)

How to Run: 
	   -$ make
	   -$ ./Sim02 <CONFIG_FILE>

	<CONFIG_FILE> - filename of configuration data with extension .conf

Requirements: must have configData file (.conf) and metaData file (.mdf) in folder

////////////////////////////////////////////////////////////////////////
Sim04 Requirements located in: 
		Scheduling Algorithms:  simFuncs.cpp @lines 1048-1320
								process.cpp @lines 133-210
Other changes: 
		readData.cpp: additional reading in for memory allocation

///////////////////////////////////////////////////////////////////////

Files Included:
	Sim02 - Main Driver: calls major functions within main implementation
		@line 47: check for command line argument
		@line 54: check that config data file ends in .conf
		@line 61: get configData
		@line 67: get metaData
		@line 73: split metaData into processes
		@line 76: log data to file

	simFuncs.cpp - Simulator Helper: Implements Functions to output data to file/monitor
		@line 87: split data into processes
		@line 157: log data to appropriate file/monitor
		@line 243: log process data, using timers and threading
		@line 344: gets beginning string to output (process times), use semaphores and mutex
		@line 480: gets the end string, what process is happening
		@line 545: timer implementation
		@line 610: runner implementation for threading
		@line 667: adds delay of thread timer to total time elapsed
		@line 698: helper function that gets time to elapse/wait
		@line 747: allocates memory based on memory size and block size
		@line 812: converts from decimal to hexidecimal
	
	readData.cpp - Helper File: Implements reading of configData and metaData
		@line 70: reads configdata from file
		@line 359: helper function, removes beginning spaces from string
		@line 397: helper function, checks if string is empty
		@line 432: reads metaData from file provided
		@line 495: helper function, parses through metaData strings
		@line 535: helper function, parses through single metaData
		@line 667: checks that logFile (metaData) file extension ends in .mdf
		
	data.h - Header File: Defines configData and metaData classes
		
	data.cpp - Implementation File: Implements member functions of configData and metaData
		@line 53: configData constructor
		@line 96: configData destructor
		@line 125: metaData constructor
		@line 156: metaData destructor
		@line 183: checks that the meta descriptor read in is valid, calculates time
		@line 269: gets endString for output
		@line 360: gets startString for output
		@line 461: checks for error in metadata

	process.h - Header File: Defines PCB struct and process class
		
	process.cpp - Implementation File: Implements process class member functions
		@line 54: process constructor, sets process pcb to new
		@line 82: process destructor, sets process pcb to terminated
		@line 110: changes state of process pcb
	

