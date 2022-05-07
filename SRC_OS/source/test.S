    .thumb
    .syntax unified

    .globl pcBranch

pcBranch:
    ldr     lr, =pCounter
	ldr		lr, [lr]            //This is crazy
    mov     r0, #5              //This moves r0
    bx      lr