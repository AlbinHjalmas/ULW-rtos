	.thumb
	.syntax unified

    .global	SaveContext
	.global	LoadContext
	.extern Running
	.align 2


/////////////////////////////////////////////////////////////////////////////
//  void SaveContext(void)
/////////////////////////////////////////////////////////////////////////////
	.thumb_func
SaveContext:
	push	{r0, r1}
    ldr 	r0, =Running	        		// Load address to context
    ldr 	r0, [r0]
    add 	r0, r0, #4                
    stmia 	r0, {r1-r12}					// Save registers r1-r12
    mov 	r1, lr
    str 	r1, [r0, #52]					// Save LR to TCB->PC
          
    mrs 	r1, xPSR						// Load CPSR into r1
    str 	r1, [r0, #56]					// and save to TCB->SPSR 
    sub 	r0, r0, #4						// Makes r0 point to context->r0
    ldr 	r1, [sp]						// Loads r0 from stack 
    str 	r1, [r0]						// Saves r0
         
    add 	r1, sp, #8						// Fetch Stackpointer
    str 	r1, [r0, #52]					// and save to TCB->SP	  
    pop		{r0, r1}                  	  
    bx 		lr                        		// Return to C-program

/////////////////////////////////////////////////////////////////////////////
// void LoadContext(void)
// author		Albin Hjalmas
// date			1/31/2017
/////////////////////////////////////////////////////////////////////////////
	.thumb_func
LoadContext:
	ldr 	r0, =Running			// r0->Running->r0
    ldr 	r0, [r0]				// r0->Running.r0
	add		r0, r0, 4				// r0->Running.r1
	mov		sp, r0					// sp->Running.r1
	pop		{r1-r12}				// load registers r1-r12 from Running
	ldr		lr, [sp, #4]			// load Running.PC into lr	
	ldr		r0, [sp, #8]			// load Running.SPSR into r0
	msr		xPSR, r0				// load r0 into xPSR
	ldr		r0, [sp, #-52]			// load Running.r0 into r0
	ldr		sp, [sp]				// load Running.SP into sp
	cpsie	i						// enable interrupts
	bx		lr

	.end