//////////////////////////////////////////////////////////////////////////////
/// This file defines the application programming interface to the kernel.
/// Please define what architecture you intend to run the kernel on.
/// for texas dsp simply define "texas dsp", for ARM cortex-m processors 
/// (no FPU support) please define "_CORTEX_M_", for x86 architecture
/// Please define "X86". If you intend to run this kernel on x86 architecture
/// please instatiate a new thread that periodically calls the timerISR(); 
/// routine. 
/// 
/// @brief	Defines the application programming interface to the kernel.
/// @file kernel.c
/// @author Albin Hjalmas.
/// @date 1/25/2017
/// @copyright Albin Hjalmas 2017. All rights reserved.
/// @license This software is released under the 
///			 GNU general public license version 3 (GNUGPLv3).
/// @see https://www.gnu.org/licenses/gpl-3.0.en.html
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
///								Includes
//////////////////////////////////////////////////////////////////////////////
#include "kernel.h"
#include "OSList.h"
#include "OS_malloc.h"

#ifdef _X86_
#include <Windows.h>
#include <process.h>

#elif _CORTEX_M_
#include "stm32f4xx.h"	
#endif

//////////////////////////////////////////////////////////////////////////////
///							Private variables
//////////////////////////////////////////////////////////////////////////////
/// @brief	The system ticks, should be incremented on a periodic basis.
static uint osTicks;

/// @brief	OSList's.
static OSList_t* readyList = NULL;
static OSList_t* waitingList = NULL;
static OSList_t* timerList = NULL;

#define SEND_WAIT 0xF1
#define SEND_NO_WAIT 0xF2

#define UNINITIALIZED 2
/// @brief	The operation mode.
static uint opMode = UNINITIALIZED;


/// @brief	The currently running task.
TCB* Running;
listobj* runningListobj;

bool isrOnState = false;

//////////////////////////////////////////////////////////////////////////////
//							Macros
//////////////////////////////////////////////////////////////////////////////

///
/// @def	initTask(listob, fnBody, deadline);
///
/// @brief	A macro that defines init task.
///
/// @author	Albin Hjalmas.
/// @date	1/30/2017
///
/// @param	listob  	The listob.
/// @param	fnBody  	The body.
/// @param	deadline	The deadline.
///
#define initTask(listob, fnBody, deadline) \
				listob->pTask->PC = fnBody;	\
				listob->pTask->SP = &(listob->pTask->StackSeg[STACK_SIZE - 1]); \
				listob->pTask->DeadLine = deadline; \

///
/// @def	setRunningTask(listob);
///
/// @brief	A macro that sets the Running* task.
///
/// @author	Albin Hjalmas.
/// @date	1/30/2017
///
/// @param	listob	The listobj* that contains the pTask.
///
#define setRunningTask(listob) \
				Running = listob->pTask; \
				runningListobj = listob; \

//////////////////////////////////////////////////////////////////////////////
///							Private functions
//////////////////////////////////////////////////////////////////////////////

///
/// @fn	static void schedulingUpdate(void)
///
/// @brief	Scheduling update.
///
/// @author	Albin Hjalmas.
/// @date	1/30/2017
///
static void schedulingUpdate(void)
{
	// Check timerList for tasks ready for execution.
	listobj* tmp = OSList_peek(timerList);
	while(tmp != NULL)
	{ // Traverse the list
		if (tmp->nTCnt <= osTicks)
		{ // Task is ready for execution
			OSList_readyInsert(readyList, OSList_getFirst(timerList));
			tmp = OSList_peek(timerList);
		}
		else // If the first element in timerList is not
			 // ready for execution then no elements in timerList
			 // are ready for execution because the list is sorted.
		{
			break;
		}
	}

	// Check waitinglist for expired deadlines
	// If a task with expired deadline is found
	// Then this task will be transferred to the readyList.
	// And the deadline will be updated.
	tmp = OSList_peek(waitingList);
	while (tmp != NULL)
	{
		if (tmp->pTask->DeadLine <= osTicks)
		{  // The deadLine is reached
			OSList_readyInsert(readyList, OSList_getFirst(waitingList));
			tmp = OSList_peek(waitingList);
		}
		else // If the first element in waitingList has not
		{	 // reached its deadline then none of the elements has.
			break;
		}
	}

	// Set the currently running task
	setRunningTask(OSList_peek(readyList));
}

///
/// @fn	static void idleTask(void)
///
/// @brief	Idle task.
///
/// @author	Albin Hjalmas.
/// @date	1/30/2017
///
static void idleTask(void)
{
	while (true)
	{
		if (Running->PC != idleTask)
		{ // This means that the timer interrupt has 
		  // induced a context switch
			isr_off();		// disable interrupts
			LoadContext();  // load context and reenable interrupts
		}
	}
}


#ifdef _X86_
///
/// @fn	void timerInterrupt(void)
///
/// @brief	Timer interrupt thread.
///
/// @author	Albin Hjalmas
/// @date	1/30/2017
///
void timerInterrupt(void)
{
	while (true)
	{
		// Sleep for 20ms
		Sleep(20);

		if (isrOnState)
		{ // Only execute this if interrupts is turned on.
			osTicks++;
			schedulingUpdate();
		}
	}
}
#elif _CORTEX_M_
void SysTick_Handler(void)
{
	osTicks++;
	schedulingUpdate();
}
#endif

//////////////////////////////////////////////////////////////////////////////
///				Task administration Function definitions.
//////////////////////////////////////////////////////////////////////////////

///
/// @fn	int init_kernel(void);
///
/// @brief	Init kernel.
///
/// @date	1/30/2017
///
/// @return	FAIL if failed or SUCCESS if successful.
///
exception init_kernel(void)
{
	osTicks = 0;

	// Create readyList
	if ((readyList = OSList_create()) == NULL)
	{ // Unable to allocate memory for list.
		return FAIL;
	}

	// Create waitingList
	if ((waitingList = OSList_create()) == NULL)
	{ // Unable to allocate memory for list.
		free(readyList);
		return FAIL;
	}

	// Create timerList
	if ((timerList = OSList_create()) == NULL)
	{ // Unable to allocate memory for list.
		free(readyList);
		free(waitingList);
		return FAIL;
	}

	// Create the idle task
	listobj* idleTaskOb = OSList_createListobj();
	if (idleTaskOb == NULL)
	{ // Something went wrong
		free(readyList);
		free(waitingList);
		free(timerList);
		return FAIL;
	}

	// Initialize the idle task
	initTask(idleTaskOb, idleTask, UINT32_MAX);

	if (!OSList_readyInsert(readyList, idleTaskOb))
	{ // Something went wrong!
		free(readyList);
		free(waitingList);
		free(timerList);
		free(idleTaskOb);
		return FAIL;
	}

	// Set the currently running task
	setRunningTask(idleTaskOb);

	// Set kernel operating mode.
	opMode = INIT;

	return SUCCESS;
}

///
/// @fn	exception create_task(void(*body)(), uint d)
///
/// @brief	Creates a task, Requires that init_kernel() have been executed.
///
/// @author	Albin Hjalmas.
/// @date	1/30/2017
///
/// @param [in,out]	body	If non-null, the body.
/// @param 		   	d   	The deadline.
///
/// @return	FAIL or SUCCESS.
///
exception create_task(void(*body)(), uint d)
{
	// must pass a valid function
	// body. and delay time cannot be 0.
	// Also check if the OSList's has been properly
	// initialized.
	if (body == NULL || d == 0 || readyList == NULL 
		|| waitingList == NULL || timerList == NULL
		|| opMode == UNINITIALIZED || d < 0)
	{
		return FAIL;
	}

	// Create the task
	listobj* task = OSList_createListobj();
	if (task == NULL) // Unable to allocate memory.
	{
		return FAIL;
	}

	// Initialize task
	initTask(task, body, d);

	if (opMode == INIT)
	{ // Just add task to ready list.
		if (!OSList_readyInsert(readyList, task)) 
		{ // Something went wrong!
			free(task);
			return FAIL;
		}
	}
	else // opMode == RUNNING
	{
		isr_off();								// disable interrupts
		volatile bool firstExecution = true;	// Create variable on stack
		SaveContext();							// Save current context

		// Load context will start running from here
		// use firstExecution to make shure that execution
		// wont get stuck in an endless loop.
		if (firstExecution)
		{
			firstExecution = !firstExecution;

			// Insert task in ready-list
			if (!OSList_readyInsert(readyList, task)) 
			{ // Something went wrong!
				free(task);
				return FAIL;
			}
			
			// Perform scheduling update
			schedulingUpdate();
			
			// Load context and reenable interrupts
			LoadContext();
		}
	}

	// The creation of a task was successful!
	return SUCCESS;
}

///
/// @fn	void terminate(void)
///
/// @brief	Terminates the currently running task.
///
/// @author	Albin Hjalmas
/// @date	1/30/2017
///
void terminate(void)
{
	// Disable interrupts
	isr_off();

	// Check os operating mode
	if (opMode != RUNNING || Running == NULL) 
	{ // The os is not running, so there is nothing to terminate
		return;
	}

	OSList_remove(readyList, runningListobj); // Remove currently running task from readylist
	//free(&(runningListobj->pTask)); // Deallocate the TCB
	free(runningListobj); // Deallocate listobject
	setRunningTask(OSList_peek(readyList)); // Set running task to be the next in readylist.
	
	// Switch to new task
	LoadContext();
}

///
/// @fn	void run(void)
///
/// @brief	Starts the kernel.
///
/// @author	Albin Hjalmas.
/// @date	1/30/2017
///
void run(void)
{
	// Disable interrupts
	isr_off();
	
	// Check that the kernel has been properly
	// initialized before continuing with this function.
	if (opMode != INIT || readyList == NULL || waitingList == NULL ||
		timerList == NULL || readyList->size == 0)
	{
		return;
	}

	// Initialize timer interrupt
#ifdef _X86_
	_beginthread(timerInterrupt, 1000, NULL);
#elif _CORTEX_M_
	uint32_t prioritygroup = 0x00U;
	prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(prioritygroup, 0, 0));
	SysTick_Config(SystemCoreClock / 50); // 20ms 
#endif

	// Set the Running* pointer to the task
	// with the earliest deadline.
	setRunningTask(OSList_peek(readyList));

	// Set the kernel operating mode to RUNNING
	opMode = RUNNING;

	// Enable interrupts
	isr_on();

	// Jump to current task.
	LoadContext();
}

//////////////////////////////////////////////////////////////////////////////
///							Timing function definitions.
//////////////////////////////////////////////////////////////////////////////

///
/// @fn	exception wait( uint nTicks );
///
/// @brief	Causes the calling task to wait for a period of nTicks.
/// @param	nTicks	Waiting period.
///
/// @return	An exception.
///
exception wait(uint nTicks)
{
	if (nTicks == 0)
	{ // No reason to wait for nothing
		return FAIL;
	}

	isr_off();		// Disable interrupts
	volatile bool firstExecution = true;
	SaveContext();

	if (firstExecution)
	{
		firstExecution = !firstExecution;
		OSList_remove(readyList, runningListobj); // Remove current task from readyList
		OSList_timerInsert(timerList, runningListobj, nTicks); // Place current task in timerList
		schedulingUpdate(); // initiate context-switch
		LoadContext(); // Commit context-switch
	}
	else if (Running->DeadLine <= ticks()) // Deadline reached
	{
		return DEADLINE_REACHED;
	} 

	return SUCCESS;
}

///
/// @fn	void set_ticks( uint no_of_ticks );
///
/// @brief	Sets the current system ticks.
/// @param	no_of_ticks	The value to set the current system ticks to.
///
void set_ticks(uint nTicks)
{
	osTicks = nTicks;	//Set new value on osTicks
}

///
/// @fn	uint ticks( void );
///
/// @brief	Gets the current system ticks.
/// @return	Current system ticks.
///
uint ticks(void)
{
	return osTicks;
}

///
/// @fn	void incTicks(void);
///
/// @brief	Increment ticks.
///
/// @author	Albin hjalmas.
/// @date	1/26/2017
///
void incTicks(void)
{
	osTicks++;			//Increment osTicks
}

///
/// @fn	uint deadline( void );
///
/// @brief	Gets the current tasks deadline.
///
/// @return	current tasks deadline.
///
uint deadline(void)
{
	if (Running != NULL)
	{
		return Running->DeadLine;
	}
	else
	{
		return 0;
	}
}

///
/// @fn	void set_deadline( uint nNew );
///
/// @brief	Sets the current tasks deadline.
///
/// @param	nNew	The new deadline.
///
void set_deadline(uint nNew)
{
	// Do nothing if there is no running task.
	if (Running == NULL)
	{
		return;
	}

	isr_off();		// Disable interrupts
	volatile bool firstExecution = true;
	SaveContext();

	if (firstExecution)
	{
		firstExecution = !firstExecution;
		listobj* tmp = OSList_getFirst(readyList); // Get the currently running task from readyList
		tmp->pTask->DeadLine = nNew;
		OSList_readyInsert(readyList, tmp);
		schedulingUpdate(); // initiate context-switch
		LoadContext(); // Commit context-switch and reenable interrupts
	}
}

//////////////////////////////////////////////////////////////////////////////
///							Intertask communication
//////////////////////////////////////////////////////////////////////////////

///
/// @fn	mailbox* create_mailbox(uint nMessages, uint nDataSize)
///
/// @brief	Creates a mailbox
///
/// @author	Albin Hjalmas
/// @date	2/12/2017
///
/// @param	nMessages	The messages.
/// @param	nDataSize	Size of the data.
///
/// @return	Null if it fails, else the new mailbox.
///
mailbox* create_mailbox(uint nMessages, uint nDataSize)
{
	// Check parameters
	if (nMessages == 0 || nDataSize == 0)
	{
		return NULL;
	}

	mailbox* res = (mailbox*)calloc(1, sizeof(mailbox));
	if (res == NULL)
	{ // Memory allocation failed.
		return NULL;
	}

	// Setup mailbox
	res->nDataSize = nDataSize;
	res->nMaxMessages = nMessages;

	// Allocate memory for head-node
	res->pHead = (msg*)calloc(1, sizeof(msg));
	if (res->pHead == NULL)
	{
		free(res); // Dont forget to free previously allocated memory.
		return NULL;
	}

	// Allocate memory for tail-node.
	res->pTail = (msg*)calloc(1, sizeof(msg));
	if (res->pTail == NULL)
	{
		free(res->pHead); // Dont forget to free previously allocated memory.
		free(res);
		return NULL;
	}

	// Setup head and tail
	res->pHead->pNext = res->pTail;
	res->pTail->pPrevious = res->pHead;
	
	return res;
}

///
/// @fn	exception remove_mailbox(mailbox* mBox)
///
/// This call will remove the Mailbox if it is empty and return
/// OK.Otherwise no action is taken and the call will return
/// NOT_EMPTY.
/// 
/// @brief	Removes the mailbox described by mBox.
///
/// @author	Albin Hjalmas
/// @date	2/15/2017
///
/// @param [in,out]	mBox	If non-null, the box.
///
/// @return	An exception.
///
exception remove_mailbox(mailbox* mBox)
{
	// Check arguments
	if (mBox == NULL)
	{
		return FAIL;
	}
	else if (mBox->nMessages == 0 && mBox->nBlockedMsg == 0)
	{ // Remove the mailbox
		free(mBox->pHead);
		free(mBox->pTail);
		free(mBox);
	}
	else
	{
		return NOT_EMPTY;
	}
}

///
/// @fn	exception send_wait(mailbox* mBox, void* pData)
///
/// @brief	Sends a message to the specified mailbox.
///
/// @author	Albin Hjalmas
/// @date	2/12/2017
///
/// @param [in,out]	mBox 	If non-null, the box.
/// @param [in,out]	pData	If non-null, the data.
///
/// @return	An exception.
///
exception send_wait(mailbox* mBox, void* pData)
{
	// mailbox and data must be non-null 
	// mailbox cannot contain asynchronous messages
	// when trying to send synchronous messages
	if (mBox == NULL || pData == NULL || mBox->nMessages != 0)
	{
		return FAIL;
	}

	isr_off();
	volatile bool firstExecution = true;
	SaveContext();

	if (firstExecution)
	{
		firstExecution = !firstExecution;

		if (mBox->nBlockedMsg < 0)
		{ // The mailbox contains receiving messages
			msg* tmp = mBox->pHead->pNext; // Get the first message added to the mailbox

			memcpy(tmp->pData, pData, mBox->nDataSize); // Copy message
			mBox->nBlockedMsg++;

			// Remove message from mailbox
			tmp->pNext->pPrevious = tmp->pPrevious;
			tmp->pPrevious->pNext = tmp->pNext;
			tmp->pNext = NULL;
			tmp->pPrevious = NULL;
			// Set the message status i.e. tell
			// receiving task that the message was 
			// successfully sent.
			tmp->Status = SUCCESS;
			OSList_remove(waitingList, tmp->pBlock);	// Remove receiving task from waitingList
			OSList_readyInsert(readyList, tmp->pBlock); // Add receiving task to readyList
		}
		else // Put a new message in the mailbox.
		{
			// Allocate new message.
			msg* tmp = (msg*)calloc(1, sizeof(msg));
			if (tmp == NULL)
			{
				while (true) {}; // memory allocation failed
			}

			tmp->pBlock = runningListobj;
			runningListobj->pMessage = tmp;
			tmp->pData = (char*)pData;

			// Add new message to mailbox
			tmp->pPrevious = mBox->pTail->pPrevious;
			tmp->pNext = mBox->pTail;
			mBox->pTail->pPrevious->pNext = tmp;
			mBox->pTail->pPrevious = tmp;
			mBox->nBlockedMsg++;

			// Move current task from readyList to
			// waitingList
			OSList_remove(readyList, runningListobj);
			OSList_waitingInsert(waitingList, runningListobj);
		}

		// Execute possible context-switch
		schedulingUpdate();
		LoadContext();
	}
	else if (ticks() >= deadline()) // Deadline is reached
	{
		isr_off();

		//Remove message from mailbox
		msg* tmp = mBox->pHead->pNext;
		while (tmp->pNext != NULL)
		{
			if (tmp == runningListobj->pMessage)
			{ // If currently running task has a reference to tmp
			  // Then remove it from the mailbox.
				tmp->pPrevious->pNext = tmp->pNext;
				tmp->pNext->pPrevious = tmp->pPrevious;
				tmp->pNext = NULL;
				tmp->pPrevious = NULL;
				tmp->pBlock = NULL;
				runningListobj->pMessage = NULL;
				free(tmp);
				mBox->nBlockedMsg--;
				break;
			}

			// Update pointer.
			tmp = tmp->pNext;
		}

		// Enable interrupts again.
		isr_on();
		return DEADLINE_REACHED;
	}
	else
	{
		return SUCCESS;
	}
}

///
/// @fn	exception receive_wait(mailbox* mBox, void* pData)
///
/// @brief	Receive wait.
///
/// @author	Albin Hjalmas
/// @date	2/15/2017
///
/// @param [in,out]	mBox 	If non-null, the box.
/// @param [in,out]	pData	If non-null, the data.
///
/// @return	An exception.
///
exception receive_wait(mailbox* mBox, void* pData)
{
	// Check parameters
	if (mBox == NULL || pData == NULL)
	{
		return FAIL;
	}

	isr_off();
	volatile bool firstExecution = true;
	SaveContext();

	// Avoid endless loop due to SaveContext
	if (firstExecution)
	{
		firstExecution = !firstExecution;

		// Check for messages ready to be received
		if (mBox->nBlockedMsg > 0 || mBox->nMessages > 0)
		{
			msg* tmp = mBox->pHead->pNext;

			// Copy message
			memcpy(pData, tmp->pData, mBox->nDataSize);

			// Remove sending message from mailbox
			tmp->pNext->pPrevious = tmp->pPrevious;
			tmp->pPrevious->pNext = tmp->pNext;
			tmp->pPrevious = NULL;
			tmp->pNext = NULL;

			// If it was a send_wait message
			if (mBox->nBlockedMsg > 0)
			{
				mBox->nBlockedMsg--;

				// Remove sending task from waitingList
				// And add it to readyList
				OSList_remove(waitingList, tmp->pBlock);
				OSList_readyInsert(readyList, tmp->pBlock);
			}
			else // It is a send_no_wait message.
			{
				mBox->nMessages--;
			}

			// Remove reference from task to message
			// And then return allocated memory
			tmp->pBlock->pMessage = NULL;
			free(tmp);
		}
		else // No sending messages waiting in mailbox
		{
			// Allocate new message.
			msg* tmp = (msg*)calloc(1, sizeof(msg));
			if (tmp == NULL)
			{
				while (true) {}; // memory allocation failed
			}

			tmp->pBlock = runningListobj;
			runningListobj->pMessage = tmp;
			tmp->pData = (char*)pData;

			// Add new message to mailbox
			tmp->pPrevious = mBox->pTail->pPrevious;
			tmp->pNext = mBox->pTail;
			mBox->pTail->pPrevious->pNext = tmp;
			mBox->pTail->pPrevious = tmp;
			mBox->nBlockedMsg--;

			// Move current task from readyList to
			// waitingList
			OSList_remove(readyList, runningListobj);
			OSList_waitingInsert(waitingList, runningListobj);
		}

		// Execute possible context-switch
		schedulingUpdate();
		LoadContext();
	}
	else if (ticks() >= deadline()) // Deadline is reached
	{
		isr_off();

		//Remove message from mailbox
		msg* tmp = mBox->pHead->pNext;
		while (tmp->pNext != NULL)
		{
			if (tmp == runningListobj->pMessage)
			{ // If currently running task has a reference to tmp
			  // Then remove it from the mailbox.
				tmp->pPrevious->pNext = tmp->pNext;
				tmp->pNext->pPrevious = tmp->pPrevious;
				tmp->pNext = NULL;
				tmp->pPrevious = NULL;
				tmp->pBlock = NULL;
				runningListobj->pMessage = NULL;
				free(tmp);
				mBox->nBlockedMsg++;
				break;
			}

			// Update pointer.
			tmp = tmp->pNext;
		}

		// Enable interrupts again.
		isr_on();
		return DEADLINE_REACHED;
	}
	else
	{
		return SUCCESS;
	}

}

exception send_no_wait(mailbox* mBox, void* pData)
{
	// Check parameters
	if (mBox == NULL || pData == NULL)
	{
		return FAIL;
	}
}

int receive_no_wait(mailbox* mBox, void* pData)
{
	// Check parameters
	if (pData = NULL)
	{
		return FAIL;
	}
}

///
/// @fn	int no_messages(mailbox* mBox)
///
/// @brief	Returns the number of messages currently contained within the 
/// 		specified mailbox.
///
/// @author	Albin Hjalmas
/// @date	2/13/2017
///
/// @param [in,out]	mBox	If non-null, the box.
///
/// @return	An int.
///
int no_messages(mailbox* mBox)
{
	if (mBox != NULL)
	{
		return abs(mBox->nMessages) + abs(mBox->nBlockedMsg);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
///					Context related function Definitions.
//////////////////////////////////////////////////////////////////////////////

///
/// @fn	extern void isr_off(void);
///
/// @brief	Disables interrupts.
///
extern void isr_off(void)
{
	isrOnState = false;
	
#ifdef _CORTEX_M_
	__disable_irq();
#endif
}

///
/// @fn	extern void isr_on(void);
///
/// @brief	Enables interrupts.
///
extern void isr_on(void)
{
	isrOnState = true;
	
#ifdef _CORTEX_M_
	__enable_irq();
#endif
}



