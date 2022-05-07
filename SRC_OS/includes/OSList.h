//////////////////////////////////////////////////////////////////////////////
/// This file defines the linked-list data structures used by the kernel
/// to maintain the running states of the created tasks.
/// @brief	Defines the linked-list data structures utilized by the kernel.
/// @file OSList.h
/// @author Albin Hjalmas.
/// @date 1/23/2017
/// @copyright Albin Hjalmas 2017. All rights reserved.
/// @license This software is released under the 
///			 GNU general public license version 3 (GNUGPLv3).
/// @see https://www.gnu.org/licenses/gpl-3.0.en.html
//////////////////////////////////////////////////////////////////////////////

#ifndef _OSLIST_H_
#define _OSLIST_H_

//////////////////////////////////////////////////////////////////////////////
//							Includes
//////////////////////////////////////////////////////////////////////////////
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "OS_malloc.h"
#include "kernel.h"

//////////////////////////////////////////////////////////////////////////////
//							Macros
//////////////////////////////////////////////////////////////////////////////

///
/// @def	OSList_readyInsert(list, listobj);
///
/// Will insert a value into list in ascending order 
/// according to deadline.
/// 
/// @brief	A macro that defines Operating system list ready insert.
///
/// @author	Albin Hjalmas
/// @date	1/26/2017
///
/// @param	list   	The list.
/// @param	listobj	The listobj.
///
#define OSList_readyInsert(list, listobj) \
				OSList_deadlineInsert(list, listobj) \

///
/// @def	OSList_waitingInsert(list, listobj);
///
/// Will insert a value into list in ascending order
/// according to deadline.
/// 
/// @brief	A macro that defines Operating system list waiting insert.
///
/// @author	Albin Hjalmas
/// @date	1/26/2017
///
/// @param	list   	The list.
/// @param	listobj	The listobj.
///
#define OSList_waitingInsert(list, listobj) \
				OSList_deadlineInsert(list, listobj) \

///
/// @def	OSList_isEmpty(list);
///
/// @brief	Returns true if the list is empty.
///
/// @author	Albin Hjalmas.
/// @date	1/30/2017
///
/// @param	list	The list.
///
#define OSList_isEmpty(list) \
				list->size == 0 \

//////////////////////////////////////////////////////////////////////////////
//							Data structures
//////////////////////////////////////////////////////////////////////////////

///
/// An object defining the underlying frame of all three types
/// of operating system lists.
/// @struct	OSList_t
///
/// @brief	An operating system list.
///
/// @author	Albin Hjalmas
/// @date	1/24/2017
///
typedef struct {
	uint32_t size;			///<Current size of the list i.e. the number of elements.
	listobj* pHead;			///<A pointer to the frontmost element in this list.
	listobj* pTail;			///<A pointer to the last element in this list.
} OSList_t;

//////////////////////////////////////////////////////////////////////////////
//							Function prototypes
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
OSList_t*	OSList_create(void);

///
/// @fn	bool OSList_timerInsert(OSList_t* list, listobj* element, uint delay);
/// 	
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
/// @param 		   	delay  	The delaytime by what amount to delay the given task with.
///
/// @return	True if it succeeds, false if it fails.
///
bool		OSList_timerInsert(OSList_t* list, listobj* element, uint delay);

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
bool		OSList_deadlineInsert(OSList_t* list, listobj* element);

///
/// @fn	listobj* OSList_getFirst(OSList_t* list);
///
/// Returns and the first listobj from the specified list and removes
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
listobj*	OSList_getFirst(OSList_t* list);

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
bool		OSList_remove(OSList_t* list, listobj* element);

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
listobj*	OSList_peek(OSList_t* list);

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
listobj*	OSList_createListobj(void);

#endif //_OSLIST_H_
