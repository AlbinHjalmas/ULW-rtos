//////////////////////////////////////////////////////////////////////////////
/// This file defines the application programming interface to the kernel.
/// Please define what architecture you intend to run the kernel on.
/// for texas dsp simply define "texas dsp", for ARM cortex-m processors 
/// (no FPU support) please define "_CORTEX_M_", for x86 architecture
/// Please define "X86". If you intend to run this kernel on x86 architecture
/// please instatiate a new thread that periodically calls the timerISR(); 
/// routine. 
/// @file kernel.h
/// @author Albin Hjalmas.
/// @date 2/8/2017
/// @copyright Albin Hjalmas 2017. All rights reserved.
/// @license This software is released under the 
/// GNU general public license version 3 (GNUGPLv3).
/// @see https://www.gnu.org/licenses/gpl-3.0.en.html
//////////////////////////////////////////////////////////////////////////////
#ifndef _KERNEL_H_
#define _KERNEL_H_

#ifdef __cplusplus
extern "C" {
#endif

// Debug option
//#define       _DEBUG


//////////////////////////////////////////////////////////////////////////////
//								Includes
//////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>


//////////////////////////////////////////////////////////////////////////////
//							Architecture related defines
//	Please note that you have to provide a architecture-specific asm-file
//	to handle the saving and loading of current cpu context.
//////////////////////////////////////////////////////////////////////////////
#ifdef texas_dsp						///< If defined: this kernel will execute on texas_dsp architecture
#define CONTEXT_SIZE    34-2			///< Number of general purpose registers
#define STACK_SIZE		100

#elif _CORTEX_M_						///< If defined: this kernel will execute on Cortex-m architecture
#define CONTEXT_SIZE    13				///< Number of general purpose registers
#define STACK_SIZE      200				///< Size of task stack

#elif _X86_								///< If defined: this kernel will execute on x86 architecture
#define CONTEXT_SIZE	8				///< Number of general purpose registers
#define STACK_SIZE		100000			///< Size of task stack

#endif

//////////////////////////////////////////////////////////////////////////////
//							Defines
//////////////////////////////////////////////////////////////////////////////

// Boolean states.
//#define TRUE    1						///<Boolean state true
//#define FALSE   !TRUE					///<Boolean state false

//Operating modes.
#define RUNNING 1						///<Operating mode Kernel is running.
#define INIT    !RUNNING				///<Operating mode Kernel is running initialization routine.

//Return messages used by os-functions.
#define FAIL    0						///<Return message: Operation failed.
#ifndef _CORTEX_M_
#define SUCCESS 1						///<Return message: Operation succeeded.
#endif
#define OK      1						///<Return message: Operation succeeded.

//misc
#define DEADLINE_REACHED        0		///<This tasks deadline has been reached.
#define NOT_EMPTY               0		///<There are messages in the mailbox.

//Who put the msg item into the mailbox ...
#define SENDER          +1				///<It was a sender who wants to send a message.
#define RECEIVER        -1				///<It was a receiver who wants to receive a message.

//////////////////////////////////////////////////////////////////////////////
///							Typedefs
//////////////////////////////////////////////////////////////////////////////
///
/// @typedef	int exception
/// @brief	Defines an alias representing the exception.
///
typedef int             exception;

///
/// @typedef	int bool
/// @brief	Defines an alias representing the bool.
///
//typedef int             bool;	 // Currently not used! (include stdbool.h instead)	

///
/// @typedef	unsigned int uint
/// @brief	Defines an alias representing an unsigned integer.
///
typedef unsigned int    uint;

///
/// @typedef	int action
/// @brief	Defines an alias representing the action.
///
typedef int 			action;



//////////////////////////////////////////////////////////////////////////////
///						Task control blocks (TCB) for
///							various architectures.
//////////////////////////////////////////////////////////////////////////////
#ifdef texas_dsp
///
/// @struct	TCB
///
/// @brief	A Task control block in accordance to 
/// 		the context provided on the texas dsp.
///
typedef struct
{
	void	(*PC)();				///<A pointer to the next line of code to be executed.
	uint	*SP;					///<Apointer to this tasks TOS (Top of stack)
	uint	Context[CONTEXT_SIZE];	///<This tasks context i.e. the register contents. 
	uint	StackSeg[STACK_SIZE];	///<This tasks stack.
	uint	DeadLine;				///<This tasks deadline.
} TCB;

#elif _CORTEX_M_

///
/// @struct	TCB
///
/// Please make shure that this TCB corresponds to the context on
/// The cpu on which you intend to run the OS on.
/// 
/// @brief	A Task control block in accordance to
/// 		the context provided on the ARM Cortex-m processors.
///
typedef struct
{
	uint	r0;
	uint	r1;
	uint	r2;
	uint	r3;
	uint	r4;
	uint	r5;
	uint	r6;
	uint	r7;
	uint	r8;
	uint	r9;
	uint	r10;
	uint	r11;
	uint	r12;
	
    uint    *SP;						///<Apointer to this tasks TOS (Top of stack)
    void    (*PC)();					///<A pointer to the next line of code to be executed.
    uint    SPSR;						///<The current status flags for this task.
    uint    StackSeg[STACK_SIZE];		///<This tasks stack.
    uint    DeadLine;					///<This tasks deadline.
} TCB;

#elif _X86_

///
/// @struct	TCB
///
/// @brief	A Task control block in accordance to
/// 		the context provided on the x86 architecture.
///
typedef struct {
	uint    Context[CONTEXT_SIZE];			///<This tasks context i.e. the register contents. 
	uint*   SP;								///<Apointer to this tasks TOS (Top of stack)
	void    (*PC)(void);					///<A pointer to the next line of code to be executed.
	uint    StackSeg[STACK_SIZE];			///<This tasks stack.
    uint    DeadLine;						///<This tasks deadline.
} TCB;

#else
#error "OS Error: No architecture specified"
#endif

//////////////////////////////////////////////////////////////////////////////
///							OS-related objects.
//////////////////////////////////////////////////////////////////////////////

struct  l_obj;					// Forward declaration

///
/// @struct	msgobj
/// A message object used by both receiver and sender to receive/send messages thru
/// a mailbox.
/// 
/// @brief	A msgobj.
///
typedef struct msgobj {
        char            *pData;				///<A pointer to the data contained in this message.
        exception       Status;				///<Status indication for this message.
        struct l_obj    *pBlock;			///<A pointer to the creator of the message.
        struct msgobj   *pPrevious;			///<A pointer to the previous message in the mailbox.
        struct msgobj   *pNext;				///<A pointer to the next message in the mailbox.
} msg;

///
/// @struct	mailbox
/// @brief	A mailbox.
///
typedef struct {
        msg             *pHead;				///<The frontmost message in this mailbox.
        msg             *pTail;				///<The last message in this mailbox.
        int             nDataSize;			///<The size in bytes of messages in this mailbox.
        int             nMaxMessages;		///<The maximum number of messages permitted in this mailbox.
        int             nMessages;			///<The actual number of messages in this mailbox.
        int             nBlockedMsg;		///<The number of messages waiting to be received/sent.
} mailbox;

///
/// @struct	l_obj
/// @brief	Defines an item stored in the OSList_t lists.
///
typedef struct l_obj {
         TCB            *pTask;				///<A pointer to this listobjects task.
         uint           nTCnt;				///<Sorting argument used in 
         msg            *pMessage;			///<A pointer back to the message belonging to this task.
         struct l_obj   *pPrevious;			///<Previous task in list.
         struct l_obj   *pNext;				///<Next task in list.
} listobj;

/*
///
/// @struct	list
/// @see OSList.h
/// @brief	A list.
///
typedef struct {
         listobj        *pHead;
         listobj        *pTail;
} list;
*/

//////////////////////////////////////////////////////////////////////////////
///							Function prototypes.
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
///					Task administration Function prototypes.
//////////////////////////////////////////////////////////////////////////////
exception	init_kernel(void);
exception	create_task( void (* body)(), uint d );
void            terminate(void);
void            run(void);


////////////////////////////////////////////////////////////////////////////
/// 					Communication function prototypes.
////////////////////////////////////////////////////////////////////////////

mailbox*	create_mailbox( uint nMessages, uint nDataSize );
exception	remove_mailbox(mailbox* mBox);

// Returns the number of messages in specified mailbox
int         no_messages( mailbox* mBox );

exception   send_wait( mailbox* mBox, void* pData );
exception   receive_wait( mailbox* mBox, void* pData );

exception	send_no_wait( mailbox* mBox, void* pData );
int         receive_no_wait( mailbox* mBox, void* pData );


//////////////////////////////////////////////////////////////////////////////
///							Timing function prototypes.
//////////////////////////////////////////////////////////////////////////////

///
/// @fn	exception wait( uint nTicks );
///
/// @brief	Causes the calling task to wait for a period of nTicks.
/// @param	nTicks	Waiting period.
///
/// @return	An exception.
///
exception	wait( uint nTicks );

///
/// @fn	void set_ticks( uint no_of_ticks );
///
/// @brief	Sets the current system ticks.
/// @param	no_of_ticks	The value to set the current system ticks to.
///
void        set_ticks( uint nTicks );

///
/// @fn	uint ticks( void );
///
/// @brief	Gets the current system ticks.
/// @return	Current system ticks.
///
uint        ticks( void );

///
/// @fn	void incTicks(void);
///
/// @brief	Increment ticks.
///
/// @author	Albin hjalmas.
/// @date	1/26/2017
///
void		incTicks(void);

///
/// @fn	uint deadline( void );
///
/// @brief	Gets the current tasks deadline.
///
/// @return	current tasks deadline.
///
uint		deadline( void );

///
/// @fn	void set_deadline( uint nNew );
///
/// @brief	Sets the current tasks deadline.
///
/// @param	nNew	The new deadline.
///
void		set_deadline( uint nNew );

//////////////////////////////////////////////////////////////////////////////
///					Context related function prototypes.
//////////////////////////////////////////////////////////////////////////////

///
/// @fn	extern void isr_off(void);
///
/// @brief	Disables interrupts.
///
extern void     isr_off(void);

///
/// @fn	extern void isr_on(void);
///
/// @brief	Enables interrupts.
///
extern void     isr_on(void);

///
/// @fn	extern void SaveContext(void);
///
/// @brief	Saves the context.
///
extern void SaveContext(void);

///
/// @fn	extern void LoadContext(void);
///
/// @brief	Loads the context.
///
extern void LoadContext(void);
	
#ifdef __cplusplus
}
#endif

#endif //_KERNEL_H_
