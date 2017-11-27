// Program Information /////////////////////////////////////////////////////////
/**
  * @file process.cpp
  *
  * @brief implements PCB and process class   
  * 
  * @details defines member functions
  *
  * @version 1.01 Carli DeCapito
  *          Scheduiling Algorithm Functionality (11/14/17)
  *
  *          1.00 Carli DeCapito
  *	  		   Original Document (10/4/17)
  *
  *
  * @note None
  */
//precompiler directives
#ifndef PROCESS_CPP
#define PROCESS_CPP

//constants
static const int NEW = 0;
static const int READY = 1;
static const int RUNNING = 2;
static const int WAITING = 3;
static const int TERMINATED = 4;

//header files
#include "process.h"
#include <iostream>

using namespace std;

//process implementation///////////////////////////////////
/**
 * @brief constructor
 *
 * @details initalizes process
 *          
 * @pre None
 *
 * @post state is set to ready
 *
 * @par Algorithm 
 *      function call
 *      
 * @exception None
 *
 * @param [in] None
 *
 * @param [out] None
 *
 * @return None
 *
 * @note None
 */
process::process()
{
  //set PCB to new
	pcb.processState = NEW;

  //set priority to 0
  priority = 0;

  processNum = 0;

  lastProc = false;

  //set time to 0
  time = 0;
}

/**
 * @brief destructor
 *
 * @details resets pcb to terminate
 *          
 * @pre none
 *
 * @post pcb state is set to terminate
 *
 * @par Algorithm 
 *      Function call
 *      
 * @exception None
 *
 * @param [in] None
 *
 * @param [out] None
 *
 * @return None
 *
 * @note None
 */
 
process::~process()
{
	pcb.processState = TERMINATED;
}


/**
 * @brief changeState
 *
 * @details assigns pcb state to state
 *          
 * @pre none
 *
 * @post pcb state is set to state 
 *
 * @par Algorithm 
 *      function call
 *      
 * @exception None
 *
 * @param [in] state provides int value of new state
 *
 * @param [out] None
 *
 * @return None
 *
 * @note None
 */
void process::changeState( int state )
{
	pcb.processState = state;
}




///////////////////////////////////////////
//       SIM04 IMPLEMENTATION            //
///////////////////////////////////////////

/**
 * @brief getTotalTime
 *
 * @details saves number of metadata actions to the process time member
 *          
 * @pre metadata exists
 *
 * @post time contains number of metadata elements to a process
 *
 * @par Algorithm 
 *      loop trhough metadata. increment time 
 *      
 * @exception None
 *
 * @param [in] None
 *
 * @param [out] None
 *
 * @return None
 *
 * @note None
 */
void process::getTotalTime()
{
  metaData metaTemp;
  int index = 0;
  int size = metadata.size();

  for ( index = 0; index < size; index++ )
  {
    metaTemp = metadata[ index ];
    time++;
  }
}


/**
 * @brief getPriority
 *
 * @details saves number of I/O metadata operations to priority
 *          
 * @pre metadata exists
 *
 * @post priority contains number of I/O actions
 *
 * @par Algorithm 
 *      loop through metadata. increment priority every time I/O occurs
 *      
 * @exception None
 *
 * @param [in] None
 *
 * @param [out] None
 *
 * @return None
 *
 * @note None
 */
void process::getPriority()
{
  metaData metaTemp;
  int index = 0;
  int size = metadata.size();

  for ( index = 0; index < size; index++ )
  {
    metaTemp = metadata[ index ];

    if( metaTemp.metaCode == 'I' || metaTemp.metaCode == 'O' )
    {
      priority++;
    }
  }
}

#endif