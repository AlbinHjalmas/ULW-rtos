#pragma once

#ifndef _OSLIST_TEST_H_
#define _OSLIST_TEST_H_
//////////////////////////////////////////////////////////////////////////////
//								Includes
//////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "../../includes/OSList.h"

//////////////////////////////////////////////////////////////////////////////
//								Prototypes
//////////////////////////////////////////////////////////////////////////////

///
/// @fn	void OSList_runTests(void);
///
/// @brief	Operating system list run tests.
///
/// @author	Albin Hjalmas.
/// @date	1/30/2017
///
void OSList_runTests(void);

///
/// @fn	void OSList_create_test(void);
///
/// @brief	Tests operating system list create.
///
/// @author	Albin hjalmas.
/// @date	1/26/2017
///
void OSList_create_test(void);

///
/// @fn	bool OSList_timerInsert_test(void)
///
/// @brief	Tests operating system list timer insert.
///
/// @author	Albin Hjalmas.
/// @date	1/26/2017
///
void OSList_timerInsert_test(void);

///
/// @fn	void OSList_deadlineInsert_test(void);
///
/// @brief	Tests operating system list deadline insert.
///
/// @author	Albin Hjalmas
/// @date	1/26/2017
///
void OSList_deadlineInsert_test(void);

///
/// @fn	void OSList_getFirst_test(void);
///
/// @brief	Tests operating system list get first.
///
/// @author	Albin Hjalmas
/// @date	1/27/2017
///
void OSList_getFirst_test(void);

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
void OSList_remove_test(void);


#endif //_OSLIST_TEST_H_

