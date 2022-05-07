//////////////////////////////////////////////////////////////////////////////
/// This file defines the test suite for the kernel.
/// Please define what architecture you intend to run the kernel on.
/// for texas dsp simply define "texas dsp", for ARM cortex-m processors 
/// (no FPU support) please define "_CORTEX_M_", for x86 architecture
/// Please define "X86". If you intend to run this kernel on x86 architecture
/// please instatiate a new thread that periodically calls the timerISR(); 
/// routine. 
/// 
/// @brief	Defines the application programming interface to the kernel_test.
/// @file kernel_test.c
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
#include "kernel_test.h"
#include "OSList_test.h"

//////////////////////////////////////////////////////////////////////////////
///							Private functions
//////////////////////////////////////////////////////////////////////////////
void task01(void);
void task02(void);
void task03(void);

//////////////////////////////////////////////////////////////////////////////
///							Private variables
//////////////////////////////////////////////////////////////////////////////
static mailbox* mb;

//////////////////////////////////////////////////////////////////////////////
///							Function definitions
//////////////////////////////////////////////////////////////////////////////

void kernel_test_run(void)
{
	puts("Testing OSList:");
	// Test the lists
	OSList_runTests();
	puts("-		OK!\n\n");

	puts("Testing OS Task administration Functions:");
	puts("- testing init_kernel() when memory allocation is disabled ...");
	// Test to initialize kernel when
	// memory allocation is mal-functioning
	OS_malloc_setPeriod(1);
	assert(init_kernel() == FAIL);
	puts("-		OK!");

	puts("- testing run() before kernel has been properly initialized ...");
	// test to run before kernel
	// is initialized properly
	// if faulty this will probably induce some strange error
	run();
	puts("-		OK!");

	puts("- testing create_task() before kernel has been properly initialized ...");
	// Try to create task before kernel initialize is called
	OS_malloc_setPeriod(OS_MALLOC_DONT_FAIL);
	assert(create_task(task01, 10) == FAIL);
	puts("-		OK!");

	puts("- properly initializing kernel and creating a task with bad argument ...");
	// Properly initialize kernel and then
	// try to create a task with bad parameters
	assert(init_kernel() == SUCCESS);
	assert(create_task(NULL, 10) == FAIL);
	puts("-		OK!");
	
	puts("- properly creating a task ...");
	// Properly create task
	assert(create_task(task01, 100) == SUCCESS);
	puts("-		OK!");

	puts("Now call run()");
	// Branch to task01
	run();
}

//////////////////////////////////////////////////////////////////////////////
//								Tasks
//////////////////////////////////////////////////////////////////////////////

void task01(void)
{

	puts("Running task1 ...\n\n");

	volatile bool state = false;
	char msg[40];

	if ((mb = create_mailbox(1, strlen(msg))) == NULL)
	{
		while (true); // Memory allocation failed
	}

	puts("Testing Intertask communication:");
	puts("- testing to block (send_wait()) until deadline is reached...");
	if (send_wait(mb, msg) == DEADLINE_REACHED)
	{
		state = true; 
	}
	// If Deadline was not reached when calling send_wait()
	// the the assert below will fail.
	assert(state);
	puts("-		OK!");

	// Reset the deadline
	set_deadline(ticks() + 15);

	// Create task02
	// Will start execution of task02 due
	// to closer deadline
	if (create_task(task02, ticks() + 10) == FAIL)
	{
		while (true); // Memory allocation failed
	}
	
	char recMsg[40];

	puts("- task1 is now to receive message from task2 ...");
	// Will receive message directly
	if (receive_wait(mb, recMsg) == DEADLINE_REACHED)
	{
		terminate(); // Something went wrong
	}

	// Display received message
	puts(recMsg);

	puts("- testing to wait for a message (receive_wait())");
	puts("- task1 is now waiting for a message from task2 ...");
	// Will wait for message to be received
	if (receive_wait(mb, recMsg) == DEADLINE_REACHED)
	{
		terminate(); // Something went wrong
	}

	// Display received message
	puts(recMsg);

	while (true)
	{
		wait(10);
		set_deadline(ticks() + 15);
	}
}

void task02(void)
{

	char* msg = "-		OK! (this is the message from task2)";

	puts("- testing send_wait() from task2 to task1, blocking operation...");
	// Send message to task1, will branch to task1
	if (send_wait(mb, msg) == DEADLINE_REACHED)
	{
		terminate();
	}
	wait(1);

	puts("- task2 is now sending message to task1 ...");
	// Send message to task1, will branch to task1
	if (send_wait(mb, msg) == DEADLINE_REACHED)
	{
		terminate();
	}

	while (true)
	{
		wait(10);
		set_deadline(ticks() + 15);
	}
}



