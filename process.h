// Program Information /////////////////////////////////////////////////////////
/**
  * @file process.h
  *
  * @brief   definition file of PCB and process class    
  * 
  * @details defines member functions and data members
  *
  * @version 1.01 Carli DeCapito
  *			 Member Functions Addition ( 11/14/17)
  *
  * 		 1.00 Carli DeCapito
  *		     Original Document (10/4/17)
  *
  * @note None
  */
//precompiler directives
#ifndef PROCESS_H
#define PROCESS_H

//header files
#include <queue>
#include <vector>
#include "data.h"

using namespace std;


//PCB struct definition
struct PCB
{
	int processState;
};

//process class definition
class process
{
	public:
		//constructor
		process();
		//destructor
		~process();

		//member functions
		void changeState( int state );
		void getTotalTime();
		void getPriority();

		//data members
		vector<metaData> metadata;
		PCB pcb;
		int processNum;
		int priority;
		int time;
		bool lastProc;
};

#endif