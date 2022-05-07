//////////////////////////////////////////////////////////////////////////////
/// @brief	Defines hook functions for malloc/calloc that can be used to induce errors into application.
/// @file OS_malloc.h
/// @author Albin Hjalmas.
/// @date 2/8/2017
/// @copyright Albin Hjalmas 2017. All rights reserved.
/// @license This software is released under the 
/// GNU general public license version 3 (GNUGPLv3).
/// @see https://www.gnu.org/licenses/gpl-3.0.en.html
//////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef _MALLOC_HOOK_H_
#define _MALLOC_HOOK_H_
//////////////////////////////////////////////////////////////////////////////
//								Includes
//////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////
//								Defines
//////////////////////////////////////////////////////////////////////////////

// If this define is made then malloc/calloc will be replaced by
// so called hook functions that intentionally will introduce faults 
// in memory allocation.
#define USE_MALLOC_HOOK

// If MALLOC_HOOK is defined to for example 9
// then every ninth malloc/calloc will fail.
#define MALLOC_PERIOD 0

#define OS_MALLOC_DONT_FAIL 0

//////////////////////////////////////////////////////////////////////////////
//								Prototypes
//////////////////////////////////////////////////////////////////////////////

///
/// @fn	void* OS_malloc(size_t size);
///
/// @brief	Operating system malloc.
///
/// @author	Albin Hjalmas
/// @date	2/8/2017
///
/// @param	size	The size.
///
/// @return	Null if it fails, else a pointer to a void.
///
void* OS_malloc(size_t size);

///
/// @fn	void* OS_calloc(size_t num, size_t size);
///
/// @brief	Operating system calloc.
///
/// @author	Albin Hjalmas
/// @date	2/8/2017
///
/// @param	num 	Number of.
/// @param	size	The size.
///
/// @return	Null if it fails, else a pointer to a void.
///
void* OS_calloc(size_t num, size_t size);

///
/// @fn	void OS_malloc_setPeriod(unsigned int period);
///
/// @brief	Set the fail-period for the malloc function, wont fail if set to 0.
///
/// @author	Albin Hjalmas
/// @date	2/11/2017
///
/// @param	period	The period.
///
void OS_malloc_setPeriod(unsigned int newPeriod);

#endif // _MALLOC_HOOK_H_