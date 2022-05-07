//////////////////////////////////////////////////////////////////////////////
/// This file defines the linked-list data structures used by the kernel
/// to maintain the running states of the created tasks.
/// @brief	Defines the linked-list data structures utilized by the kernel.
/// @file OSList.c
/// @author Albin Hjalmas.
/// @date 1/23/2017
/// @copyright Albin Hjalmas 2017. All rights reserved.
/// @license This software is released under the 
/// GNU general public license version 3 (GNUGPLv3).
/// @see https://www.gnu.org/licenses/gpl-3.0.en.html
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//							Macros
//////////////////////////////////////////////////////////////////////////////

///
/// @def	addWhenZero(list, element);
///
/// @brief	A macro that defines add when zero.
///
/// @author	Albin Hjalmas.
/// @date	1/26/2017
///
/// @param	list   	The list.
/// @param	element	The element.
///
#define addWhenZero(list, element) \
					list->pHead = element; \
					list->pTail = element; \

///
/// @def	addInFront(list, element);
///
/// @brief	A macro that defines add in front.
///
/// @author	Albin Hjalmas.
/// @date	1/26/2017
///
/// @param	list   	The list.
/// @param	element	The element.
///
#define addInFront(list, element) \
					list->pHead->pPrevious = element; \
					element->pNext = list->pHead; \
					list->pHead = element; \

///
/// @def	addInBack(list, element);
///
/// @brief	A macro that defines add in back.
///
/// @author	Albin Hjalmas
/// @date	1/26/2017
///
/// @param	list   	The list.
/// @param	element	The element.
///
#define addInBack(list, element) \
					list->pTail->pNext = element; \
					element->pPrevious = list->pTail; \
					list->pTail = element; \

///
/// @def	addAfter(object, element);
///
/// @brief	Adds element after object in the linked list.
///
/// @author	Albin Hjalmas.
/// @date	1/26/2017
///
/// @param	object 	The object.
/// @param	element	The element.
///
#define insertAfter(object, element) \
					object->pNext->pPrevious = element; \
					element->pNext = object->pNext; \
					object->pNext = element; \
					element->pPrevious = object; \
					

//////////////////////////////////////////////////////////////////////////////
//							Includes
//////////////////////////////////////////////////////////////////////////////
#include "OSList.h"

//////////////////////////////////////////////////////////////////////////////
//							Function definitions
//////////////////////////////////////////////////////////////////////////////

///
/// @fn	OSList_t* OSList_create(void);
///
/// @brief	Operating system list create.
///
/// @author	Albin Hjalmas
/// @date	1/24/2017
///
/// @return	Null if it fails, else an empty list ready to be used.
///
OSList_t* OSList_create(void)
{
	// Allocate memory for the list.
	return (OSList_t*)OS_calloc(1, sizeof(OSList_t));
}

///
/// @fn	bool OSList_timerInsert(OSList_t* list, listobj* element, uint delay);
/// Inserts a task into the timer list. The timer list is sorted according
/// to nTcnt which when this function is called is assigned the value: 
/// nTcnt = ticks + delay. where ticks refers to the system ticks - a
/// 32-Bit variable that is incremented periodically.
/// The list is sorted in ascending order.
/// 
/// @brief	Timer list insert.
///
/// @author	Albin Hjalmas.
/// @date	1/26/2017
///
/// @param [in,out]	list   	If non-null, the list which to insert to.
/// @param [in,out]	element	If non-null, the element to insert into the list.
/// @param 		   	delay  	The number of ticks to delay the given task with.
///
/// @return	True if it succeeds, false if it fails.
///
bool OSList_timerInsert(OSList_t* list, listobj* element, uint delay)
{
	// Check parameters
	if (list == NULL || element == NULL || delay == 0)
	{
		return false;
	}

	// Calculate nTcnt
	element->nTCnt = ticks() + delay;

	if (list->size == 0)
	{
		addWhenZero(list, element);
	}
	else if (element->nTCnt < list->pHead->nTCnt)
	{
		addInFront(list, element);
	}
	else if (element->nTCnt >= list->pTail->nTCnt)
	{
		addInBack(list, element);
	}
	else
	{
		listobj* tmp = list->pHead;
		
		// Traverse the list until the position for which to insert element is found
		for (int i = 0; i < (list->size - 1) && tmp->pNext->nTCnt <= element->nTCnt; i++, tmp = tmp->pNext) {};

		// Insert element after the listobj
		// pointed to by tmp
		insertAfter(tmp, element);
	}


	// Increment list size
	list->size++;
	return true;
}

///
/// @fn	bool OSList_deadlineInsert(OSList_t* list, listobj* element);
///
/// Inserts a listobject in ascending order according to the deadline. 
/// 
/// @brief	Operating system list ready insert.
///
/// @author	Albin Hjalmas
/// @date	1/26/2017
///
/// @param [in,out]	list   	If non-null, the list.
/// @param [in,out]	element	If non-null, the element.
///
/// @return	True if it succeeds, false if it fails.
///
bool OSList_deadlineInsert(OSList_t* list, listobj* element)
{
	// Check parameters
	if (list == NULL || element == NULL)
	{
		return false;
	}

	if (list->size == 0)
	{
		addWhenZero(list, element);
	}
	else if (element->pTask->DeadLine < list->pHead->pTask->DeadLine)
	{
		addInFront(list, element);
	} 
	else if (element->pTask->DeadLine >= list->pTail->pTask->DeadLine)
	{
		addInBack(list, element);
	} 
	else
	{
		listobj* tmp = list->pHead;

		// Traverse the list until the position for which to insert element is found
		for (int i = 0; 
			i < (list->size - 1) && tmp->pNext->pTask->DeadLine <= element->pTask->DeadLine; 
			i++, tmp = tmp->pNext) {};

		// Insert element after the listobj
		// pointed to by tmp
		insertAfter(tmp, element);
	}


	// Increment list size
	list->size++;
	return true;
}

///
/// @fn	listobj* OSList_getFirst(OSList_t* list);
///
/// Returns the first listobj from the specified list and removes
/// it from the list. if no object is present it will return NULL.
/// 
/// @brief	Returns the first listobj in the specified list.
///
/// @author	Albin Hjalmas
/// @date	1/27/2017
///
/// @param [in,out]	list	If non-null, the list.
///
/// @return	Null if it fails, else a pointer to a listobj.
///
listobj* OSList_getFirst(OSList_t* list)
{

	listobj* tmp;

	if (list == NULL || list->size == 0)
	{
		return NULL;
	} 
	else if (list->size == 1)
	{
		tmp = list->pHead;
		list->pHead = NULL;
		list->pTail = NULL;
	} 
	else
	{
		tmp = list->pHead;
		list->pHead = list->pHead->pNext;
		list->pHead->pPrevious = NULL;	
	}
	
	tmp->pNext = NULL;
	tmp->pPrevious = NULL;
	list->size--;
	return tmp;
}

///
/// @fn	bool OSList_remove(OSList_t* list, listobj* element);
///
/// @brief	Operating system list remove.
///
/// @author	Albin Hjalmas.
/// @date	1/30/2017
///
/// @param [in,out]	list   	If non-null, the list.
/// @param [in,out]	element	If non-null, the element.
///
/// @return	True if it succeeds, false if it fails.
///
bool OSList_remove(OSList_t* list, listobj* element)
{
	// Check argument
	if (list == NULL || list->size == 0 || element == NULL)
	{ // faulty arguments
		return false;
	}

	if (list->pHead == element && list->size == 1)
	{
		list->pHead = NULL;
		list->pTail = NULL;
	} 
	else if (list->pHead == element && list->size > 1)
	{
		list->pHead = list->pHead->pNext;
		list->pHead->pPrevious = NULL;
		element->pNext = NULL;
	}
	else if (list->pTail == element && list->size > 1)
	{
		list->pTail = list->pTail->pPrevious;
		list->pTail->pNext = NULL;
		element->pPrevious = NULL;
	}
	else
	{
		listobj* tmp = list->pHead->pNext;
		while (tmp != NULL)
		{
			if (tmp == element)
			{
				tmp->pPrevious->pNext = tmp->pNext;
				tmp->pNext->pPrevious = tmp->pPrevious;
				tmp->pNext = NULL;
				tmp->pPrevious = NULL;
				break;
			}
			tmp = tmp->pNext;
		}

		if (tmp == NULL)
		{
			return false;
		}
	}


	list->size--;
	return true;
}

///
/// @fn	listobj* OSList_peek(OSList_t* list);
///
/// Returns a pointer to the first element in this list
/// without removing the first element.
/// 
/// @brief	Returns a pointer to the first element in this list.
///
/// @author	Albin Hjalmas
/// @date	1/27/2017
///
/// @param [in,out]	list	If non-null, the list.
///
/// @return	Null if it fails, else a pointer to a listobj.
///
listobj* OSList_peek(OSList_t* list)
{
	if (list == NULL)
	{
		return NULL;
	}

	return list->pHead;
}

///
/// @fn	listobj* OSList_createListobj(void);
///
/// @brief	Operating system list create listobj.
///
/// @author	Albin hjalmas
/// @date	1/26/2017
///
/// @return	Null if it fails, else a pointer to a listobj.
///
listobj* OSList_createListobj(void)
{
	listobj* tmp = (listobj*)OS_calloc(1, sizeof(listobj));
	if (tmp == NULL)
	{
		return NULL;
	}

	tmp->pTask = (TCB*)OS_calloc(1, sizeof(TCB));
	if (tmp->pTask == NULL)
	{ // calloc returned NULL
		free(tmp);
		return NULL;
	}


	return tmp;
}
