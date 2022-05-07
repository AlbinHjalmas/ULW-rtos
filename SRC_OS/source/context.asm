
.686P							;Target processor.  Use all available instructions (x86)
.MODEL FLAT, C					;Use the flat memory model. Use C calling conventions

;Externally defined variables
EXTERN Running:DWORD			;A pointer to the currently running task's Tcb_t
EXTERN isrOnState:DWORD			;A pointer to the interrupt state

.DATA							;Create a near data segment.
	EAXTMP	DWORD	0			;Temporary storage for EAX register
	EBXTMP	DWORD	0			;Temporary storage for EBX register
	ESPTMP  DWORD   0			;Temporary storage for stack pointer
	RETURN  DWORD   0			;Temporary return address storage
	PCTMP	DWORD	0			;Temporary program counter storage

.CODE							;Indicates the start of a code segment.

;****************************************************************************************
; Saves the current context into the current 
; TCB pointed to by Running.
;****************************************************************************************
PUBLIC SaveContext
SaveContext PROC
	;Store return address
	MOV [EAXTMP], EAX			;Store EAX before using it
	MOV EAX, Running			;Load current Tcb_t 
	MOV [EBXTMP], EBX			;Store EBX before using it
	POP EBX						;POP return address into EBX
	MOV [EAX+8*4+4], EBX		;Store return address into runningTask.PC
	MOV [RETURN], EBX			;Store return address

	;Make sp point to context segment
	ADD EAX, 8*4				;Point to last address in context segment (stack grows downwards)
	MOV [ESPTMP], ESP			;Store stack pointer before changing it
	MOV ESP, EAX				;Change SP to point to Context
	MOV EAX, [EAXTMP]			;Restore EAX
	MOV EBX, [EBXTMP]			;Restore EBX

	;Save context
	PUSH EAX
	PUSH EBX
	PUSH ECX
	PUSH EDX
	PUSH ESI
	PUSH EDI
	PUSH EBP
	PUSHF

	;Return to callee
	MOV ESP, [ESPTMP]			;Restore stack pointer
	MOV EAX, Running
	MOV [EAX+8*4], ESP
	MOV EAX, [EAXTMP]
	PUSH [RETURN]
	RET
SaveContext ENDP 


;*******************************************************************************************
; Loads the current context into the current 
; TCB pointed to by Running.
;*******************************************************************************************
PUBLIC LoadContext
LoadContext PROC
	;Prepare PC, SP and pointer to Context
	MOV EAX, Running			;Load stackpointer into EBX
	MOV EBX, [EAX+8*4]			;
	MOV [ESPTMP], EBX			;Store stackpointer
	MOV EBX, [EAX+9*4]			;Load PC into EBX
	MOV [PCTMP], EBX			;Store PC
	ADD EAX, 2					;EAX now points to context
	MOV ESP, EAX				;Stackpointer now points to Context

	;Load context
	POPF						;Pop all the flags
	POP EBP
	POP EDI
	POP ESI
	POP EDX
	POP ECX
	POP EBX
	POP EAX

	;Set stack pointer
	MOV ESP, [ESPTMP]			;Load stack pointer
	PUSH [PCTMP]				;Push PC onto stack

	;Set interrupt state
	MOV isrOnState, 1

	RET							;Will pop PC from stack and branch
LoadContext ENDP
END