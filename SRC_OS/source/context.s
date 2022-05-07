	IMPORT Running
	EXPORT SetXPSR
	EXPORT LoadContext
	EXPORT SaveContext
	
	AREA context_code, CODE, READONLY
  
;****************************************************************************
;  void SetXPSR(uint32_t val);
;***************************************************************************
SetXPSR PROC
	PUSH	{R0}
	MOV		R0, #0
	MSR 	IPSR, R0
	POP		{R0}
	BX 		LR
	ENDP
  
;****************************************************************************
;  void SaveContext(void)
;***************************************************************************
SaveContext PROC 
    STMDB SP!,{r0,r1}
    ;--savecontext--
    LDR 	R0, =Running	        	; Load address to context
    LDR 	R0, [R0]
    ADD 	R0, R0, #4                
    STMIA 	R0, {R1-R12}	            ; Save registers r1-r12 
    MOV 	R1, LR
    STR 	R1, [R0, #52]               ; Save LR to TCB->PC
          
    MRS 	R1, PSR                     ; Load CPSR into r1
    STR 	R1, [R0, #56]               ; and save to TCB->SPSR
    SUB 	R0, R0, #4                  ; Makes r0 point to context->r0
    LDR 	R1, [SP]                    ; Loads r0 from stack
    STR 	R1, [R0]		            ; Saves r0. 
         
    ADD 	R1,SP,#8                    ; Fetch Stackpointer
    STR 	R1,[R0,#52]                 ; and save to TCB->SP  	  
    LDMIA 	SP!,{R0,R1}                  	  
    BX 		LR                        	; Return to C-program
	ENDP
    
;****************************************************************************
;  void LoadContext(void)
;***************************************************************************    
LoadContext PROC
    LDR 	R0, =Running
    LDR 	R0, [R0]
		
    LDR 	R1, [R0, #52]               ; Catch Running-> SP
    SUB 	SP, SP, #8	                ; Find a unused stack area
    STMDA 	SP!, {R1}                   ; and put SP on the temporary stack 
    LDR 	R1, [R0, #4]                ; Fetch r0's value.TCB->context[1]
    STMDA 	SP!, {R1}                   ; push r1's value to stack       
    LDR 	R1, [R0]                    ; Fetch r0's value.TCB->context[0]
    STMDA 	SP!, {R1}                   ; push r0's value to stack      
       
    ADD 	R0, R0, #4            
    LDMIA 	R0!, {R1-R12}               ; Restore values for r1-r12          

    LDR 	R0, =Running
    LDR 	R0, [R0]    
    ADD 	R0, R0, #0x38            
    LDR 	R14, [R0]

    CMP 	R14, #0
    BEQ 	trap
    LDR 	R0, [R0, #8]                 ; Load value for SPSR
    CMP 	R0, #0                       ; If SPSR = 0, first loading.
    BEQ 	skipSPSR                     ; then skip loading SPSR.
    MSR 	PSR, R0                      ; else CPSR = TCB->SPSR
skipSPSR          
	LDR 	R0,	=Running
	LDR 	R0,	[R0]             
	LDR 	R13, [R0, #0x38]

	POP 	{R0, R1}
	LDR 	SP, [R13, #4] 
	BX 		LR
trap
      B .
   ENDP
	   
   ALIGN 4
   END	