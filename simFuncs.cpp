// Program Information ////////////////////////////////////
/**
  * @file simFuncs.cpp
  *
  * @brief implementation of simulation, simulates 
  *        processes and threading    
  * 
  * @details splits meta data into processes and outputs data
  *
  * @version 1.11 Carli DeCapito
  *			 Schuduling Algorithms ( 11/ 13/17 )
  *
  *			 1.10 Carli DeCapito
  *			 Mutex/Semaphore Functionality (10/18/17)
  *  
  *          1.01 Carli DeCapito
  * 		 Timer Functionality ( 10/7/17 )
  *
  * 		 1.00 Carli DeCapito
  *		     Original Document (10/6/17)
  *
  * @note None
  */

// Header Files
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include "data.h"
#include "process.h"
#include <semaphore.h>

using namespace std;

//global variables
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
bool FIRST_ALLOC = true;
int LAST_ADD = 0;

//function definitions
void splitMetaData( vector<process> &pdata, 
					vector<metaData> mdata );

void logData( configData cdata, 
			   vector<process> pdata );

bool logProcessingData( process &proc, 
						configData cdata, 
						bool monitor, 
						bool file, 
						double &time, 
						ofstream &fout );

bool getStartString( metaData mdata, 
					 char string[], 
					 double &time, 
					 process &proc,
					 configData &cdata );

void getEndString( metaData mdata, 
				   char string[], 
				   double &time,
				   process &proc,
				   configData &cdata );

void timer( double &time,
			int timeInterval );

void* runner( void *param );

void addDelay( double &time,
			   int threadTime );

int getWaitTime( timeval startTime );

void allocateMemory( configData &cdata, 
					 char string[] );

void decToHex( int num, 
			   char string[] );

void getElapsedTime( timeval sTime, 
					 timeval eTime, 
					 int &microSecs, 
					 int &secs );

void getStartEndTimes( timeval &sTime,
					   timeval &eTime,
					   int timeInt );

/// SIM 4 Funcitons ///////////////////////////////////////
//first come first serve
void FIFO ( vector<process> pdata, 
			configData &cdata, 
			bool monitor, 
			bool file, 
			ofstream &fout );

//shortest job first
void SJF ( vector<process> pdata, 
		   configData &cdata, 
		   bool monitor, 
		   bool file, 
		   ofstream &fout );

//priority scheduling
void PS ( vector<process> pdata, 
		  configData &cdata, 
		  bool monitor, 
		  bool file, 
		  ofstream &fout );



//function implementation /////////////////////////////////
/**
 * @brief splitMetaData 
 *
 * @details splits metaData into separate processes
 *          
 * @pre meta data must have a start(S & A) and end (A) to be a process
 *
 * @post data is stored into vector
 *
 * @par Algorithm 
 *      use for loop to get data
 *      
 * @exception None
 *
 * @param [in] mdata provies vector of metadata
 *
 * @param [out] pdata provides vector of processes
 *
 * @return Void
 *
 * @note None
 */
void splitMetaData( vector<process> &pdata, 
					vector<metaData> mdata )
{
	vector<metaData> tempData;
	process tempProcess;
	metaData tempMeta;
	int index;
	int processIndex = 1;
	int metaSize = mdata.size();

	//for size of vector, push data into queue
	for ( index = 0; index < metaSize; index++ )
	{
		//extract data and push onto queue
		tempMeta = mdata[ index ];
		tempData.push_back( tempMeta );

		//check if new process start or end
		if ( tempMeta.metaCode == 'S' || tempMeta.metaCode == 'A' )
		{

			if( strcmp( tempMeta.metaDescriptor, "end" ) == 0 )
			{
				tempProcess.changeState( NEW );
				tempProcess.processNum = processIndex;
				processIndex++;
				tempProcess.metadata = tempData;

				pdata.push_back( tempProcess );

				tempData.clear();
			}
			
		}
	}
}

/**
 * @brief logData
 *
 * @details outputs data to file/monitor/both
 *          
 * @pre processes and cdata must exist
 *
 * @post data is log to designated spot
 *
 * @par Algorithm 
 *      using checks, output data
 *      
 * @exception None
 *
 * @param [in] cdata provides class of configuration data
 *
 * @param [in] pdata provides vector of proesses
 *
 * @return Void
 *
 * @note None
 */
void logData( configData cdata, 
			  vector<process> pdata )
{
	bool file = false;
	bool monitor = false;
	ofstream fout;

	if( strcmp( cdata.logInfo, "Log to Both" ) == 0 )
	{
		file = monitor = true;
	}
	else if ( strcmp( cdata.logInfo, "Log to File" ) == 0 )
	{
		file = true;
	}
	else if ( strcmp( cdata.logInfo, "Log to Monitor" ) == 0 )
	{
		monitor = true;
	}
	else
	{
		cout << "Error. Invalid Logging Information: " 
			 << cdata.logInfo << endl; 
	}
	//check if logging to file
	if( file == true )
	{
		//if file path was not given, report error and return
		if ( cdata.logFilePath[ 0 ] == '\0' )
		{
			cout << "Error. Missing File Path" << endl;
			return;
		}
		//otherwise open file
		fout.open( cdata.logFilePath );
	}

	if( strcmp( cdata.scheduling, "FIFO") == 0 )
	{
		FIFO( pdata, cdata, monitor, file, fout );
	}
	else if ( strcmp( cdata.scheduling, "PS") == 0 )
	{
		PS( pdata, cdata, monitor, file, fout );
	}
	else if ( strcmp( cdata.scheduling, "SJF") == 0 )
	{
		SJF( pdata, cdata, monitor, file, fout );
	}


	fout.close();
}


/**
 * @brief logProcessingData
 *
 * @details logs data for single process
 *          
 * @pre mdata must be in queue
 *
 * @post data is logged
 *
 * @par Algorithm 
 *      Loop until queue is empty
 *      
 * @exception None
 *
 * @param [out] proc provides current process
 *
 * @param [in] cdata provides class of configuration data
 *
 * @param [in] monitor provides bool if logging to monitor
 *
 * @param [in] file provides bool if logging to file
 *
 * @param [out] time provides total time of process
 *
 * @param [out] fout provides ofstream variable to output to file
 *
 * @return bool
 *
 * @note None
 */
bool logProcessingData( process &proc, 
						configData cdata, 
						bool monitor, 
						bool file, 
						double &time, 
						ofstream &fout )
{
	metaData currMetaData;
	bool dataValid;
	bool check = false;
	char startStr[ STR_LEN ];
	char endStr[ STR_LEN ];
	char errorStr[ STR_LEN ];
	vector<metaData> mdata = proc.metadata;

	//while no more metadata in queue
	while( mdata.size() > 0 )
	{
		//get current metadata
		currMetaData = mdata.front();
		
		mdata.erase( mdata.begin() );

		//check that the current metadata is valid
		dataValid = currMetaData.errorCheck( errorStr, cdata );

		//check that metadata is valid
		if( dataValid )
		{
			//get start string
			check = getStartString( currMetaData, startStr, time, proc, cdata );

			//output to monitor
			if( monitor ) 
			{
				if( currMetaData.metaCode == 'S' )
				{
					if( proc.processNum  == 1 )
					{
						cout << startStr << endl;
					}
					else if ( strcmp( currMetaData.metaDescriptor, "end" ) == 0 )
					{
						cout << startStr << endl;
					}
				}
				else
				{
					cout << startStr << endl;
				}
			}
			//output to file
			if( file )
			{
				if( currMetaData.metaCode == 'S' )
				{
					if( proc.processNum  == 1 )
					{
						fout << startStr << endl;
					}
					else if ( strcmp( currMetaData.metaDescriptor, "end" ) == 0 )
					{
						fout << startStr << endl;
					}
				}
				else
				{
					fout << startStr << endl;
				}
			}
			//if could get start string. then get end string
			if ( check )
			{

				getEndString( currMetaData, endStr, time, proc, cdata );

				//output to monitor
				if ( monitor )
				{
					cout << endStr << endl;
				}
				//output to file
				if ( file )
				{
					fout << endStr << endl;
				}
			}
		}
		//else error occured
		else 
		{
			cout << errorStr << endl;
			return false;
		}
	}
	return true;
}


/**
 * @brief getStartString
 *
 * @details returns starting string of output
 *          
 * @pre cdata and mdata must be valid
 *
 * @post string is passed by reference to print
 *
 * @par Algorithm 
 *      Does checks and function calls
 *      
 * @exception None
 *
 * @param [in] mdata provides metaData
 *
 * @param [in] string provides cstring to output
 *
 * @param [out] time provides total time to output
 *
 * @param [out] proc provides process class obj
 *
 * @return bool
 *
 * @note None
 */
bool getStartString( metaData mdata, 
					 char string[], 
					 double &time, 
					 process &proc,
					 configData &cdata )
{
	//initalize functions and variables
	char tempStr[ STR_LEN ];
	char timeStr[ STR_LEN ];
	pthread_t tid;
	int metaTime = mdata.time;
	int* threadTime = &metaTime;
	sem_t semaphore;
	
	//get mutex lock
	pthread_mutex_lock( &mutex );

	//get time
	sprintf( timeStr, "%f", time );
	strcpy( string, timeStr );
	strcat( string, " - " );

	//convert process number to string
	sprintf( tempStr, "%d", proc.processNum );
	
	//if end of metadata
	if( strcmp( mdata.metaDescriptor, "end" ) == 0 && mdata.metaCode == 'S' && proc.lastProc == true )
	{
		strcat( string, "OS: process " );
		strcat( string, tempStr );
		strcat( string, " completed \n" ); 

		sprintf( timeStr, "%f", time );
		strcat( string, timeStr );
		strcat( string, " - " );
	}

	//get string to output process number
	if( mdata.metaCode != 'S' && mdata.metaCode != 'A' )
	{
		strcat ( string, "Process " );
		strcat ( string, tempStr );
		strcat ( string, ": " );
	}

	//get rest of string
	mdata.getStartString( string, tempStr );

	//check for I/O metadata
	if ( mdata.metaCode == 'I' || mdata.metaCode == 'O' )
	{
		//change PCB state to waiting
		proc.changeState ( WAITING );

		//initalize semaphore
		sem_init( &semaphore, 0, 1 );

		//take semaphore
		sem_wait( &semaphore );

		//create thread to do IO 
		pthread_create( &tid, NULL, runner, threadTime );

		//wait for thread to finish
		pthread_join( tid, NULL );

		//release semaphore
		sem_post( &semaphore );

		//add delay
		addDelay( time, *threadTime );

		if ( strcmp( mdata.metaDescriptor, "printer" )  == 0 )
		{
			if( cdata.currPrinter == cdata.numPrinter + 1 )
			{
				cdata.currPrinter = 0;
			}
			strcat( string, " on PRNTR " );
			sprintf( tempStr, "%d", cdata.currPrinter );
			strcat( string, tempStr );
			cdata.currPrinter++;
		}
		else if( strcmp( mdata.metaDescriptor, "hard drive" ) == 0 )
		{
			if( cdata.currHardDrive == cdata.numHardDrive + 1 )
			{
				cdata.currHardDrive = 0;
			}
			
			strcat( string, " on HDD " );
			sprintf( tempStr, "%d", cdata.currHardDrive );
			strcat( string, tempStr );
			
			cdata.currHardDrive++;
		}
		else if( strcmp( mdata.metaDescriptor, "speaker" ) == 0 )
		{
			if( cdata.currSpeaker == cdata.numSpeaker + 1 )
			{
				cdata.currSpeaker = 0;
			}
			
			strcat( string, " on speaker " );
			sprintf( tempStr, "%d", cdata.currSpeaker );
			strcat( string, tempStr );
			
			cdata.currSpeaker++;
		}
		//change pcb state to running
		proc.changeState ( RUNNING );

		//release mutex
		pthread_mutex_unlock( &mutex );

		return true;
	}

	timer( time, mdata.time );

	if ( ( mdata.metaCode == 'S' ) ||
		 ( mdata.metaCode == 'A' && 
		 strcmp( mdata.metaDescriptor, "end" ) == 0 ) )
	{
		//realease mutex
		pthread_mutex_unlock( &mutex );
		return false;
	}

	//release mutex
	pthread_mutex_unlock( &mutex );
	return true;
}


/**
 * @brief getEndString
 *
 * @details gets string to output to file/monitor
 *          
 * @pre cdata must be valid and startstring must have outputed
 *
 * @post string is passed by reference to output
 *
 * @par Algorithm 
 *      completes various checks and calls to get appropriate string
 *      
 * @exception None
 *
 * @param [in] mdata provides metaData obj
 *
 * @param [in] string provides cstring to output
 *
 * @param [out] time provides total time elapsed for process
 *
 * @param [out] proc provides process class obj
 *
 * @param [in] cdata provides configuration data obj
 *
 * @return None
 *
 * @note None
 */
void getEndString( metaData mdata, 
				   char string[], 
				   double &time,
				   process &proc,
				   configData &cdata )
{
	char tempStr[ STR_LEN ];
	char timeStr[ STR_LEN ];
	char memStr[ STR_LEN ];

	//get time in string 
	sprintf( timeStr, "%f", time );
	strcpy( string, timeStr );
	strcat( string, " - " );

	//convert process number to string
	sprintf( tempStr, "%d", proc.processNum );

	//check that code is not S or A
	if ( mdata.metaCode != 'S' && mdata.metaCode != 'A' )
	{
		strcat( string, "Process " );
		strcat( string, tempStr );
		strcat( string, ": " );

	}
	//allocate in memory
	if( mdata.metaCode == 'M' )
	{
		//memory allocation
		cdata.firstAlloc = FIRST_ALLOC;
		cdata.lastAddUsed = LAST_ADD;
		allocateMemory( cdata, memStr );
		
	}

	//get rest of string data
	mdata.getEndString( string, tempStr, memStr );
}

/**
 * @brief timer
 *
 * @details waits for amount of time requested, and updates total time
 *          
 * @pre timeInterval must be nonnegative
 *
 * @post time is passed and then time total updated to output
 *
 * @par Algorithm 
 *      Use sys/time.h timeval structure
 * 		Use while loop until pass time. 
 *      
 * @exception None
 *
 * @param [in] timeInterval provides int with requested time to wait
 *
 * @param [out] time provides double of total time to output
 *
 * @return None
 *
 * @note None
 */
void timer( double &time,
			int timeInterval )
{
	timeval startTime, endTime; 
	int secs, usecs;
	double tempTime;

	//get current time
	getStartEndTimes( startTime, endTime, timeInterval ); 

	//calculate time elapsed
	//time in secs and usecs
	getElapsedTime( startTime, endTime, usecs, secs );

	//convert into double
	tempTime = ( double ) usecs / 1000 / 1000;
	time = time + ( double ) secs + tempTime; 
}

/**
 * @brief runner
 *
 * @details runner function to wait for thread creation
 *          
 * @pre param is time to elapse
 *
 * @post param is updated with time in usecs
 *
 * @par Algorithm 
 *      use timeval from sys/time.h library
 *      
 * @exception None
 *
 * @param [in] param provides int ptr with time to wait/ returns microsecs
 *
 * @return Void*
 *
 * @note None
 */
void* runner( void *param )
{
	int secs, usecs;
	timeval startTime, endTime;

	getStartEndTimes( startTime, endTime, *( int* ) param );

	getElapsedTime( startTime, endTime, usecs, secs );

	*( int* )param = secs * ( 1000000 ) + usecs;

	pthread_exit( 0 );
}


/**
 * @brief addDelay
 *
 * @details adds delay from thread to total time
 *          
 * @pre threadTime contains time thread elapsed
 *
 * @post time is updated
 *
 * @par Algorithm 
 *      increment time with time of thread
 *      
 * @exception None
 *
 * @param [in] threadTime provides integer value returned from runner
 *
 * @param [out] time provides int value of total time
 *
 * @return Void
 *
 * @note None
 */
void addDelay( double &time,
			   int threadTime )
{
	double temp = ( double ) threadTime / 1000 / 1000; 

	time += temp;
}


/**
 * @brief getWaitTime
 *
 * @details helper functions that gets time to wait
 *          
 * @pre startTime must be valid
 *
 * @post return time to elapse
 *
 * @par Algorithm 
 *      use timeval from sys/time.h
 *      
 * @exception None
 *
 * @param [in] startTime provides timeval of start time
 *
 * @param [out] 
 *
 * @return None
 *
 * @note None
 */
int getWaitTime( timeval startTime )
{
	timeval time;
	int usecs, secs;

	//get current time
	gettimeofday( &time, NULL );

	//extract secs and microsecs
	getElapsedTime( startTime, time, usecs, secs );

	if( secs >  0 )
	{
		usecs = usecs + ( secs * 1000000 );
	}
	return usecs;
}


/**
 * @brief allocateMemory
 *
 * @details calculates addresses of memory block
 *          
 * @pre cdata must exist
 *
 * @post returns an address in string
 *
 * @par Algorithm 
 *      checks for previously allocated mem, adds block to curr
 * 		memory 
 *      
 * @exception None
 *
 * @param [in] string provides string for address to be outputed
 *
 * @param [out] cdata provides configdata obj with data
 *
 * @return None
 *
 * @note None
 */
void allocateMemory( configData &cdata, 
					 char string[] )
{
	int address;
	char hexAddress[ STR_LEN ];
	int numZeros, length, index;

	if ( cdata.firstAlloc == true )
	{
		
		cdata.firstAlloc = FIRST_ALLOC = false;
		address = 0;
	}
	else
	{
		address = cdata.lastAddUsed + cdata.memoryBlockSize;
		if( address >= cdata.sysMemory )
		{
			address = 0;
		}

		cdata.lastAddUsed = LAST_ADD = address;
	}


	decToHex( address, hexAddress );
	
	length = strlen( hexAddress );

	numZeros = 8 - length;

	for( index = 0; index < numZeros; index++ )
	{
		string[ index ] = '0';
	}

	string[ index ] = '\0';
	strcat( string, hexAddress );


}


/**
 * @brief decToHex
 *
 * @details converts from dec to hex
 *          
 * @pre none
 *
 * @post string contains hex address
 *
 * @par Algorithm 
 *      uses mod to calculate hex value
 *      
 * @exception None
 *
 * @param [in] string provides hex string
 *
 * @param [in] num provides decimal string 
 *
 * @return None
 *
 * @note None
 */
void decToHex( int num, 
			   char string[] )
{
	int quotient = num / 16;
	int remainder = num % 16;
	int other, index;
	int inserted = 0;
	char letter;
	char temp[ STR_LEN ];

	if ( num == 0 )
	{
		string[ 0 ] = '0';
		string[ 1 ] = '\0';
		return; 
	}

	if ( remainder < 10 )
	{
		sprintf( temp, "%d", remainder );
		letter = temp[ 0 ];
	}
	else
	{
		switch( remainder )
		{
			case 10: 
				letter = 'A';
				break;

			case 11:
				letter = 'B';
				break;

			case 12:
				letter = 'C';
				break;

			case 13:
				letter = 'D';
				break;

			case 14: 
				letter = 'E';
				break;

			case 15:
				letter = 'F';
				break;
		}
	}

	string[ 0 ] = letter;
	inserted++;

	while( quotient != 0 )
	{
		other = quotient;
		quotient = quotient / 16;
		remainder = other % 16;

		if ( remainder < 10 )
		{
			sprintf( temp, "%d", remainder );
			letter = temp[ 0 ];
		}
		
		switch( remainder )
		{
			case 10: 
				letter = 'A';
				break;

			case 11:
				letter = 'B';
				break;

			case 12:
				letter = 'C';
				break;

			case 13:
				letter = 'D';
				break;

			case 14: 
				letter = 'E';
				break;

			case 15:
				letter = 'F';
				break;
		}

		for( index = inserted - 1; index >= 0; index-- )
		{
			string[ index + 1 ] = string[ index ];
		}

		string[ 0 ] = letter;
		inserted++;
	}
	string[ inserted ] = '\0';

}


/**
 * @brief getStartEndTimes
 *
 * @details get start time and end time using loop
 *          
 * @pre none
 *
 * @post sTime and eTime contain start and endtimes
 *
 * @par Algorithm 
 *      Use loop to wait until end
 *      
 * @exception None
 *
 * @param [in] timeInt provides int of time to wait 
 *
 * @param [out] sTime provides timeval value of startTime
 *			    eTime provides timeval value of endTime
 *
 * @return None
 *
 * @note None
 */
void getStartEndTimes( timeval &sTime,
					   timeval &eTime,
					   int timeInt )
{
	gettimeofday( &sTime, NULL );

	while( getWaitTime( sTime ) < timeInt );

	gettimeofday( &eTime, NULL);
}



/**
 * @brief getElapsedTime
 *
 * @details converts time elapsed since start to finish
 *          
 * @pre none
 *
 * @post usecs and secs contains the time
 *
 * @par Algorithm 
 *      Add times in usecs and secs and then account for carryover
 *      
 * @exception None
 *
 * @param [in] sTime provides startTime timeval value
 * 			   eTime provides endTime timeval value
 *
 * @param [out] microsecs provides int value of microsecs elapsed
 *			    secs provides int value of secs elapsed
 *
 * @return None
 *
 * @note None
 */
void getElapsedTime( timeval sTime, 
				     timeval eTime, 
				     int &microSecs, 
				     int &secs)
{

	//wait until time has bassed
	secs = eTime.tv_sec - sTime.tv_sec; 
	microSecs = eTime.tv_usec - sTime.tv_usec; 

	if( microSecs < 0 )
	{
		//add 1 sec worth of microsecs
		microSecs += 1000000;

		//take one sec away from secs
		secs -= 1;
	}
	//if usecs is greater than 1 sec, convert to 1 sec
	if( microSecs > 999999 )
	{
		secs += 1;
		microSecs -= 1000000;
	}
}



///////////////////////////////////////////
//		   SIM04 IMPLEMENTATION	         //
///////////////////////////////////////////
//first come first serve

/**
 * @brief FIFO 
 *
 * @details implements first in first out cpu scheduler
 *          
 * @pre pdata, cdata exists
 *
 * @post pdata is properly ordered
 *
 * @par Algorithm 
 *      Since processes are already in order, just call to logProcessingData
 *      
 * @exception None
 *
 * @param [in] pdata provides vector of process class 
 *
 * @param [in] monitor provides bool value to determine if outputting to monitor
 *
 * @param [in] file provides bool value to determine if outputting to file
 *
 * @param [out] cdata provides configuration data class 
 *
 * @param [out] fout provides ofstream variable to output to file
 *
 * @return None
 *
 * @note None
 */
void FIFO ( vector<process> pdata, 
			configData &cdata, 
			bool monitor, 
			bool file, 
			ofstream &fout )
{
	double time = 0.0;
	int index = 0;
	int size = pdata.size();
	bool valid; 
	
	pdata[ size - 1].lastProc = true;

	for ( index = 0; index < size; index++ )
	{
		pdata[ index ].changeState( RUNNING );

		valid = logProcessingData( pdata[ index ], cdata, monitor, file, time, fout );

		if( ! valid )
		{
			return;
		}
		pdata[ index ].changeState( TERMINATED );
	}

}

//shortest job first
/**
 * @brief SJF
 *
 * @details implements shortest job first cpu scheduler
 *          
 * @pre pdata, cdata exists
 *
 * @post pdata is properly ordered
 *
 * @par Algorithm 
 *      Order Processes, make sure first and last metadata remain the same
 *		Call logProcessingData
 *      
 * @exception None
 *
 * @param [in] pdata provides vector of process class 
 *
 * @param [in] monitor provides bool value to determine if outputting to monitor
 *
 * @param [in] file provides bool value to determine if outputting to file
 *
 * @param [out] cdata provides configuration data class 
 *
 * @param [out] fout provides ofstream variable to output to file
 *
 * @return None
 *
 * @note None
 */
void SJF ( vector<process> pdata, 
		   configData &cdata, 
		   bool monitor, 
		   bool file, 
		   ofstream &fout )
{
	int index, subIndex;
	int size = pdata.size();
	int msize;
	metaData mTemp1;
	metaData mTemp2;
	double time = 0.0;
	process tempProcess;
	bool valid; 

	//get total time of all processes
	for ( index = 0; index < size; index++ )
	{
		pdata[ index ].getTotalTime();
	}

	for ( index = 0; index < size; index++ )
	{
		for ( subIndex = 0; subIndex < size - 1; subIndex++ )
		{
			//swap processes if next one is greater
			if( pdata[ subIndex ].time > pdata[ subIndex + 1 ].time )
			{
				tempProcess = pdata[ subIndex ];
				pdata[ subIndex ] = pdata[ subIndex + 1 ];
				pdata[ subIndex + 1 ] = tempProcess;
			}
		}
	}

	pdata[ size - 1].lastProc = true;

	//ensures that S(end) and S(start) remain at apprioriate place 
	for( index = 0; index < size; index++)
	{
		msize = pdata[ index ].metadata.size();
		for( subIndex = 0; subIndex < msize; subIndex++ )
		{
			if( pdata[ index ].metadata[ subIndex ].metaCode == 'S' &&
				strcmp( pdata[ index ].metadata[ subIndex ].metaDescriptor, "end" ) == 0 )
			{
				mTemp1 = pdata[ index ].metadata[ subIndex ];
				pdata[ index ].metadata.erase( pdata[ index ].metadata.begin() + subIndex );
			}
			else if ( pdata[ index ].metadata[ subIndex ].metaCode == 'S' &&
				strcmp( pdata[ index ].metadata[ subIndex ].metaDescriptor, "start" ) == 0 )
			{
				mTemp2 = pdata[ index ].metadata[ subIndex ];
				pdata[ index ].metadata.erase( pdata[ index ].metadata.begin() + subIndex );
			}
		}
	}

	pdata[ 0 ].metadata.insert( pdata[ 0 ].metadata.begin(), mTemp2 );
	pdata[ size - 1 ].metadata.push_back( mTemp1 );

	for ( index = 0; index < size; index++ )
	{
		pdata[ index ].changeState( RUNNING );

		pdata[ index ].processNum = index + 1;

		valid = logProcessingData( pdata[ index ], cdata, monitor, file, time, fout );

		if( !valid )
		{
			return;
		}

		pdata[ index ].changeState( TERMINATED );
	}


}


/**
 * @brief PS
 *
 * @details implements priority cpu scheduler
 *          
 * @pre pdata, cdata exists
 *
 * @post pdata is properly ordered
 *
 * @par Algorithm 
 *      Orders processes. Ensures last and first metadata remain the same
 *		Call logProcessingData
 *      
 * @exception None
 *
 * @param [in] pdata provides vector of process class 
 *
 * @param [in] monitor provides bool value to determine if outputting to monitor
 *
 * @param [in] file provides bool value to determine if outputting to file
 *
 * @param [out] cdata provides configuration data class 
 *
 * @param [out] fout provides ofstream variable to output to file
 *
 * @return None
 *
 * @note None
 */
void PS ( vector<process> pdata, 
		  configData &cdata, 
		  bool monitor, 
		  bool file, 
		  ofstream &fout )
{
	int index, subIndex;
	int size = pdata.size();
	int msize;
	metaData mTemp1;
	metaData mTemp2;
	double time = 0.0;
	bool valid;
	process tempProcess;

	for ( index = 0; index < size; index++ )
	{
		pdata[ index ].getPriority();
	}

	for ( index = 0; index < size; index++ )
	{
		for ( subIndex = 0; subIndex < size - 1; subIndex++ )
		{
			if ( pdata[ subIndex ].priority < pdata[ subIndex + 1 ].priority )
			{
				tempProcess = pdata[ subIndex ];
				pdata[ subIndex ] = pdata[ subIndex + 1 ];
				pdata[ subIndex + 1 ] = tempProcess;
			}
		}
	}

	pdata[ size - 1].lastProc = true;

	for( index = 0; index < size; index++)
	{
		msize = pdata[ index ].metadata.size();
		for( subIndex = 0; subIndex < msize; subIndex++ )
		{
			if( pdata[ index ].metadata[ subIndex ].metaCode == 'S' &&
				strcmp( pdata[ index ].metadata[ subIndex ].metaDescriptor, "end" ) == 0 )
			{
				mTemp1 = pdata[ index ].metadata[ subIndex ];
				pdata[ index ].metadata.erase( pdata[ index ].metadata.begin() + subIndex );
			}
			else if ( pdata[ index ].metadata[ subIndex ].metaCode == 'S' &&
				strcmp( pdata[ index ].metadata[ subIndex ].metaDescriptor, "start" ) == 0 )
			{
				mTemp2 = pdata[ index ].metadata[ subIndex ];
				pdata[ index ].metadata.erase( pdata[ index ].metadata.begin() + subIndex );
			}
		}
	}

	pdata[ 0 ].metadata.insert( pdata[ 0 ].metadata.begin(), mTemp2 );
	pdata[ size - 1 ].metadata.push_back( mTemp1 );

	for ( index = 0; index < size; index++ )
	{
		pdata[ index ].changeState( RUNNING );
		pdata[ index ].processNum = index + 1;

		valid = logProcessingData( pdata[ index ], cdata, monitor, file, time, fout );

		if( !valid )
		{
			return;
		}
		pdata[ index ].changeState( TERMINATED );
	}
}
