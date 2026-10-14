
.syntax unified
.cpu cortex-m3
.thumb

.global Reset_Handler
.global Default_Handler

.extern main
.extern SVC_Handler
.extern PendSV_Handler
.extern SysTick_Handler

.section .isr_vector, "a", %progbits
.balign 4

.word _estack
.word Reset_Handler

/* Cortex-M3 System Exceptions */
.word Default_Handler     /* NMI */
.word Default_Handler     /* HardFault */
.word Default_Handler     /* MemManage */
.word Default_Handler     /* BusFault */
.word Default_Handler     /* UsageFault */

.word 0                   /* Reserved */
.word 0                   /* Reserved */
.word 0                   /* Reserved */
.word 0                   /* Reserved */

.word SVC_Handler         /* SVCall */
.word Default_Handler     /* Debug Monitor */
.word 0                   /* Reserved */
.word PendSV_Handler      /* PendSV */
.word SysTick_Handler     /* SysTick */

/* External interrupts */
.rept 60
.word Default_Handler
.endr


/* Reset Handler */
.section .text.Reset_Handler, "ax", %progbits
.type Reset_Handler, %function
.thumb_func

Reset_Handler:

    /* Copy .data from FLASH to RAM */

    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata

copy_data:
    cmp r1, r2
    bcs zero_bss

    ldr r3, [r0]
    str r3, [r1]

    adds r0, r0, #4
    adds r1, r1, #4

    b copy_data


zero_bss:

    /* Clear .bss */

    ldr r0, =_sbss
    ldr r1, =_ebss

    movs r2, #0

zero_loop:
    cmp r0, r1
    bcs call_main

    str r2, [r0]

    adds r0, r0, #4

    b zero_loop


call_main:
    bl main

hang:
    b hang

.size Reset_Handler, .-Reset_Handler


/* Default IRQ Handler */
.section .text.Default_Handler, "ax", %progbits
.type Default_Handler, %function
.thumb_func

Default_Handler:
    b Default_Handler

.size Default_Handler, .-Default_Handler
