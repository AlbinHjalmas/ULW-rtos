//////////////////////////////////////////////////////////////////////////////
/// This file tests the OSList API.
/// @brief	Defines the linked-list data structures utilized by the kernel.
/// @file OSList_test.c
/// @author Albin Hjalmas.
/// @date 1/25/2017
/// @copyright Albin Hjalmas 2017. All rights reserved.
/// @license This software is released under the 
///			 GNU general public license version 3 (GNUGPLv3).
/// @see https://www.gnu.org/licenses/gpl-3.0.en.html
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//								Includes
//////////////////////////////////////////////////////////////////////////////
#include "OSList_test.h"

//////////////////////////////////////////////////////////////////////////////
//								Variables
//////////////////////////////////////////////////////////////////////////////
// Dummy tasks
void task(void) {}		

//////////////////////////////////////////////////////////////////////////////
//								Tests
//////////////////////////////////////////////////////////////////////////////

///
/// @fn	void OSList_runTests(void);
///
/// @brief	Operating system list run tests.
///
/// @author	Albin Hjalmas.
/// @date	1/30/2017
///
void OSList_runTests(void)
{
	// Run tests
	OSList_create_test();
	OSList_timerInsert_test();
	OSList_deadlineInsert_test();
	OSList_getFirst_test();
	OSList_remove_test();
}

///
/// @fn	void OSList_create_test(void);
///
/// @brief	Tests operating system list create.
///
/// @author	Albin hjalmas.
/// @date	1/26/2017
///
void OSList_create_test(void)
{
	OSList_t* list;
	list = OSList_create();

	//Test for null-pointer.
	assert(list != NULL);

	// Clean up after test
	free(list);
}

///
/// @fn	bool OSList_timerInsert_test(void)
///
/// @brief	Tests operating system list timer insert.
///
/// @author	Albin Hjalmas
/// @date	1/26/2017
///
void OSList_timerInsert_test(void)
{
	//Turn of interrupts
	isr_off();

	// Create the list.
	OSList_t* list = OSList_create();
	assert(list != NULL);

	// The variable to test
	bool state = false;

	// Try to pass a NULL pointer
	// Should return false.
	state = true;
	state = OSList_timerInsert(list, NULL, 100);
	assert(!state);
	assert(list->size == 0); // Size of list should be 0

	
	// Initializing ob
	listobj* ob = OSList_createListobj();
	ob->pTask->PC = task;

	// Try to pass 0 delay to timer list, no point of delaying 0 ticks
	state = true;
	state = OSList_timerInsert(list, ob, 0); // Should return false
	assert(!state);
	assert(list->size == 0);
	assert(list->pHead == NULL);
	assert(list->pTail == NULL);

	// Try to pass actual object, this time the listobj* should be added
	state = false;
	state = OSList_timerInsert(list, ob, 1);
	assert(state); // State should now be true
	assert(list->size == 1); // Should be 1 object in the list
	assert(list->pHead == ob);	// Head should point to the object
	assert(list->pTail == ob); // Tail should also point to the object
	assert(list->pHead->pNext == NULL); // There is only one item in the task
	assert(list->pHead->pPrevious == NULL); // --||--

	// Try to pass another object
	// This time with a larger delay, hence the object should 
	// be added into the list "behind" the first.
	listobj* ob2 = OSList_createListobj();
	assert(ob2 != NULL);
	ob2->pTask->PC = task;
	state = false;
	state = OSList_timerInsert(list, ob2, 10);
	assert(state); // state should now be true
	assert(list->size == 2); // should be 2 objects in the list
	assert(list->pHead == ob); // head should point to the first object
	assert(list->pHead->pNext == ob2); // the object after head shold be ob2
	assert(list->pTail == ob2); // ob2 should be the last object

	// Try to pass another object
	// this time with a delay greater than ob's but smaller than ob2's
	// hence this object should end up in between ob and ob2
	listobj* ob3 = OSList_createListobj();
	assert(ob3 != NULL);
	ob3->pTask->PC = task;
	state = false;
	state = OSList_timerInsert(list, ob3, 5);
	assert(state); // state should now be true
	assert(list->size == 3); // There should now be 3 objects in the list
	assert(list->pHead->pNext == ob3); // ob3 should be added after ob
	assert(list->pTail->pPrevious == ob3); // ob3 should be added prior to ob2

	// Add values so that the list contains delays
	// from 1 - 11 ticks.
	listobj* ob11 = OSList_createListobj();
	ob11->pTask->PC = task;
	OSList_timerInsert(list, ob11, 11);

	listobj* ob6 = OSList_createListobj();
	ob6->pTask->PC = task;
	OSList_timerInsert(list, ob6, 6);

	listobj* obc = OSList_createListobj();
	obc->pTask->PC = task;
	OSList_timerInsert(list, obc, 2);

	listobj* ob9 = OSList_createListobj();
	ob9->pTask->PC = task;
	OSList_timerInsert(list, ob9, 9);

	listobj* ob4 = OSList_createListobj();
	ob4->pTask->PC = task;
	OSList_timerInsert(list, ob4, 4);

	listobj* ob7 = OSList_createListobj();
	ob7->pTask->PC = task;
	OSList_timerInsert(list, ob7, 7);

	listobj* ob8 = OSList_createListobj();
	ob8->pTask->PC = task;
	OSList_timerInsert(list, ob8, 8);

	listobj* obk = OSList_createListobj();
	obk->pTask->PC = task;
	OSList_timerInsert(list, obk, 3);

	// now check if list is sorted
	listobj* tmp = list->pHead;
	for (uint i = 1; i <= list->size; i++, tmp = tmp->pNext)
	{
		assert(i == tmp->nTCnt);
	}

	//Clean up after test
	free(list);
	free(ob);
	free(ob11);
	free(ob2);
	free(ob3);
	free(ob4);
	free(ob6);
	free(ob7);
	free(ob8);
	free(ob9);
	free(obc);
	free(obk);

	// Turn on interrupts again
	isr_on();
}

///
/// @fn	void OSList_deadlineInsert_test(void);
///
/// @brief	Tests operating system list deadline insert.
///
/// @author	Albin Hjalmas
/// @date	1/26/2017
///
void OSList_deadlineInsert_test(void)
{
	//Turn of interrupts
	isr_off();

	// Create the list.
	OSList_t* list = OSList_create();
	assert(list != NULL);

	// The variable to test
	bool state = false;

	// Try to pass a NULL pointer
	// Should return false.
	state = true;
	state = OSList_deadlineInsert(list, NULL);
	assert(!state);
	assert(list->size == 0); // Size of list should be 0

	// Initializing ob
	listobj* ob = OSList_createListobj();
	ob->pTask->PC = task;
	ob->pTask->DeadLine = 1;

	// Try to pass actual object, this time the listobj* should be added
	state = false;
	state = OSList_deadlineInsert(list, ob);
	assert(state); // State should now be true
	assert(list->size == 1); // Should be 1 object in the list
	assert(list->pHead == ob);	// Head should point to the object
	assert(list->pTail == ob); // Tail should also point to the object
	assert(list->pHead->pNext == NULL); // There is only one item in the task
	assert(list->pHead->pPrevious == NULL); // --||--

	// Try to pass another object
	// This time with a larger delay, hence the object should 
	// be added into the list "behind" the first.
	listobj* ob2 = OSList_createListobj();
	assert(ob2 != NULL);
	ob2->pTask->PC = task;
	ob2->pTask->DeadLine = 10;
	state = false;
	state = OSList_deadlineInsert(list, ob2);
	assert(state); // state should now be true
	assert(list->size == 2); // should be 2 objects in the list
	assert(list->pHead == ob); // head should point to the first object
	assert(list->pHead->pNext == ob2); // the object after head shold be ob2
	assert(list->pTail == ob2); // ob2 should be the last object

	// Try to pass another object
	// this time with a delay greater than ob's but smaller than ob2's
	// hence this object should end up in between ob and ob2
	listobj* ob3 = OSList_createListobj();
	assert(ob3 != NULL);
	ob3->pTask->PC = task;
	ob3->pTask->DeadLine = 5;
	state = false;
	state = OSList_deadlineInsert(list, ob3);
	assert(state); // state should now be true
	assert(list->size == 3); // There should now be 3 objects in the list
	assert(list->pHead->pNext == ob3); // ob3 should be added after ob
	assert(list->pTail->pPrevious == ob3); // ob3 should be added prior to ob2

	// Add values so that the list contains delays
	// from 1 - 11 ticks.
	listobj* ob11 = OSList_createListobj();
	ob11->pTask->PC = task;
	ob11->pTask->DeadLine = 11;
	OSList_deadlineInsert(list, ob11);

	listobj* ob6 = OSList_createListobj();
	ob6->pTask->PC = task;
	ob6->pTask->DeadLine = 6;
	OSList_deadlineInsert(list, ob6);

	listobj* obc = OSList_createListobj();
	obc->pTask->PC = task;
	obc->pTask->DeadLine = 2;
	OSList_deadlineInsert(list, obc);

	listobj* ob9 = OSList_createListobj();
	ob9->pTask->PC = task;
	ob9->pTask->DeadLine = 9;
	OSList_deadlineInsert(list, ob9);

	listobj* ob4 = OSList_createListobj();
	ob4->pTask->PC = task;
	ob4->pTask->DeadLine = 4;
	OSList_deadlineInsert(list, ob4);

	listobj* ob7 = OSList_createListobj();
	ob7->pTask->PC = task;
	ob7->pTask->DeadLine = 7;
	OSList_deadlineInsert(list, ob7);

	listobj* ob8 = OSList_createListobj();
	ob8->pTask->PC = task;
	ob8->pTask->DeadLine = 8;
	OSList_deadlineInsert(list, ob8);

	listobj* obk = OSList_createListobj();
	obk->pTask->PC = task;
	obk->pTask->DeadLine = 3;
	OSList_deadlineInsert(list, obk);

	// now check if list is sorted
	listobj* tmp = list->pHead;
	for (uint i = 1; i <= list->size; i++, tmp = tmp->pNext)
	{
		assert(i == tmp->pTask->DeadLine);
	}

	//Clean up after test
	free(list);
	free(ob);
	free(ob11);
	free(ob2);
	free(ob3);
	free(ob4);
	free(ob6);
	free(ob7);
	free(ob8);
	free(ob9);
	free(obc);
	free(obk);

	// Turn on interrupts again
	isr_on();
}

///
/// @fn	void OSList_getFirst_test(void);
///
/// @brief	Tests operating system list get first.
///
/// @author	Albin Hjalmas
/// @date	1/27/2017
///
void OSList_getFirst_test(void)
{
	//Disable interrupts while running test
	isr_off();

	// Create NULL list
	OSList_t* list = NULL;

	// Create listobject.
	listobj* ret = NULL;
	listobj* ob = OSList_createListobj();

	// Try to pass a NULL list
	// Return should be NULL
	ret = OSList_getFirst(list);
	assert(ret == NULL);

	// Create list
	list = OSList_create();
	assert(list != NULL); // Make shure list is not NULL.

	// Try to extract from empty list
	// Should return NULL
	ret = OSList_getFirst(list);
	assert(ret == NULL);


	// Try to add object to list
	// And extract it
	// Return should be the added object
	OSList_timerInsert(list, ob, 100);
	ret = OSList_getFirst(list);
	assert(ret == ob);
	assert(list->size == 0);
	assert(list->pHead == NULL);
	assert(list->pTail == NULL);

	// Try to add multiple objects to list 
	// and then extract them
	for (uint i = 1; i < 100; i++)
	{
		ob = OSList_createListobj();
		OSList_timerInsert(list, ob, i);
	}

	for (uint i = 1; i < 100; i++)
	{
		ob = OSList_getFirst(list);
		assert(ob->nTCnt == i);
		free(ob);
	}

	assert(list->size == 0);
	assert(list->pHead == NULL);
	assert(list->pTail == NULL);

	// Clean up after test
	free(list);

	// Enable interrupts again
	isr_on();
}

///
/// @fn	bool OSList_remove_test(OSList_t* list, listobj* element);
///
/// @brief	Tests operating system list remove.
///
/// @author	Albin Hjalmas
/// @date	1/30/2017
///
/// @param [in,out]	list   	If non-null, the list.
/// @param [in,out]	element	If non-null, the element.
///
void OSList_remove_test(void)
{
	OSList_t* list = NULL;
	listobj* ob = OSList_createListobj();

	// Test to remove from NULL list;
	assert(OSList_remove(list, ob) == false);

	// Create list and fill it
	list = OSList_create();
	assert(list != NULL); // Make shure list was created
	OSList_deadlineInsert(list, OSList_createListobj());

	listobj* tmp = OSList_createListobj();

	// Try to remove object that is not in list
	assert(OSList_remove(list, tmp) == false);
	free(tmp);

	// Try to remove from singular list
	tmp = list->pHead;
	assert(OSList_remove(list, tmp) == true);
	assert(list->size == 0);
	free(tmp);

	// Fill list
	for (uint i = 1; i <= 100; i++)
	{
		OSList_timerInsert(list, OSList_createListobj(), i);
	}

	// try to remove non-existant object
	tmp = OSList_createListobj();
	assert(OSList_remove(list, tmp) == false);
	free(tmp);

	// Try to remove head
	tmp = list->pHead;
	assert(OSList_remove(list, tmp) == true);
	assert(list->size == 99);
	assert(list->pHead != tmp);
	assert(list->pHead->pPrevious == NULL);
	assert(tmp->pNext == NULL);
	assert(tmp->pPrevious == NULL);
	free(tmp);

	// Try to remove tail
	tmp = list->pTail;
	assert(OSList_remove(list, tmp) == true);
	assert(list->size == 98);
	assert(list->pTail != tmp);
	assert(list->pTail->pNext == NULL);
	assert(tmp->pNext == NULL);
	assert(tmp->pPrevious == NULL);
	free(tmp);

	// Try to remove between head and tail
	tmp = list->pHead->pNext->pNext;
	assert(OSList_remove(list, tmp) == true);
	assert(list->pHead->pNext->pNext != tmp);
	assert(tmp->pNext == NULL);
	assert(tmp->pPrevious == NULL);
	assert(list->size == 97);
	free(tmp);


	// Clean up after test
	tmp = list->pHead->pNext;
	while (tmp != NULL)
	{
		free(tmp->pPrevious);
		tmp = tmp->pNext;
	}
	free(list->pTail);
	free(list);
}
