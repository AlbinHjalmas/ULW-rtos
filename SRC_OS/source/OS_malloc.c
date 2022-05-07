//////////////////////////////////////////////////////////////////////////////
/// @brief	Defines hook functions for malloc/calloc that can be used to induce errors into application.
/// @file OS_malloc.c
/// @author Albin Hjalmas.
/// @date 2/8/2017
/// @copyright Albin Hjalmas 2017. All rights reserved.
/// @license This software is released under the 
/// GNU general public license version 3 (GNUGPLv3).
/// @see https://www.gnu.org/licenses/gpl-3.0.en.html
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//									Includes
//////////////////////////////////////////////////////////////////////////////
#include "OS_malloc.h"

//////////////////////////////////////////////////////////////////////////////
//								Private variables
//////////////////////////////////////////////////////////////////////////////
static unsigned int cnt = 0;
static unsigned int period = 0;


//////////////////////////////////////////////////////////////////////////////
//								Function definitions
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
void* OS_malloc(size_t size)
{
	cnt++;
	if (cnt == period)
	{ // malloc/calloc period reached so return null.
		cnt = 0;
		return NULL;
	}

	return malloc(size);
}

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
void* OS_calloc(size_t num, size_t size)
{
	cnt++;
	if (cnt == period)
	{ // malloc/calloc period reached so return null.
		cnt = 0;
		return NULL;
	}

	return calloc(num, size);
}

///
/// @fn	void OS_malloc_setPeriod(unsigned int period);
///
/// Set the fail-period for the malloc function, wont fail if set to 0.
/// To use this function _DEBUG must be defined else this functionality wont
/// be available.
/// 
/// @brief	Set the fail-period for the malloc function, wont fail if set to 0.
///
/// @author	Albin Hjalmas
/// @date	2/11/2017
///
/// @param	period	The period.
///
void OS_malloc_setPeriod(unsigned int newPeriod)
{
	// Reset counter
	cnt = 0;

// Only use this functionality while debugging
#ifdef _DEBUG
	period = newPeriod;
#else
	period = 0;
#endif

}




