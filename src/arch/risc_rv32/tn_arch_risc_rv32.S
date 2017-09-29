#include "tn_cfg_dispatch.h"

/* External references */

.extern  _tn_curr_run_task
.extern  _tn_next_task_to_run
.extern  IFS0
.extern  __tn_sys_on_context_switch
.extern tn_tick_int_processing


/* Public functions declared in this file */

.global  __INT0Interrupt
.global  __tn_arch_context_switch_now_nosave
.global  __tn_arch_context_switch_pend
.global  __tn_arch_is_int_disabled
.global  __tn_arch_inside_isr
.global  __tn_p24_ffs_asm
.global  _tn_arch_sr_save_int_dis
.global  _tn_arch_sr_restore
.global  _tn_arch_int_dis
.global  _tn_arch_int_en
.global  tn_arch_sched_dis_save
.global tn_arch_sched_restore

/* constants */
.equ context_size, 128

/* function implementations*/

/**
called whenever the system timer reaches 0 and therefore
an interrupt fires to take back control from user thread.

make sure not to write more than 4 instructions within this function
since the interrupt vector can only fit 4 instructions. any additional instruction will  be 
overridden by the code of the next interrupt handler
**/
.section .timer_vector1
_tn_arch_timer_handler:
    j tn_tick_int_processing 

; _tn_arch_ecall_handler:



/*
* ------------------------------------------------------------------------
* _tn_arch_context_switch_now_nosave()
*
* Called whenever we need to switch context to new task, but don't save
* current context.
*
* Interrupts should be disabled before calling this function.
*
* See comments in tn_arch.h
*/
__tn_arch_context_switch_now_nosave:
    la x5, _tn_curr_run_task            /* x5 = &_tn_curr_run_task */     
    lw x6, _tn_next_task_to_run         /* x6 = _tn_next_task_to_run */
    lw sp, 0(x6)                        /* set sp to next task's stack */

#if _TN_ON_CONTEXT_SWITCH_HANDLER
    lw a0, 0(x5)                 /* a0 = _tn_curr_run_task */
    add a1, x6, zero             /* a1 =  _tn_next_task_to_run */

   /* 
    * arguments are set:
    * - a0: _tn_curr_run_task
    * - a1: _tn_next_task_to_run
    */
    jal __tn_sys_on_context_switch
#endif
    
    sw x6, 0(x5)    /* _tn_curr_run_task = _tn_next_task_to_run */
    jal x0, _tn_arch_sr_restore


# disables all interrupts unconditionally
_tn_arch_int_dis:
    CSRRWI zero, 0x4, 0x1 # disable all interrupts
    ret

# enables all interrupts uncondtionally
_tn_arch_int_en:
    CSRRWI x0, 0x4, 0x1 #  enable gie flag
    ret

/* 
 check if interrupts are disabled
 return 1 if interrupts are enabled, 0 otherwise
 */
_tn_arch_is_int_disabled:
    CSRRS a0,0x4, zero   #read interrupt status
    andi  a0,a0,0x1      #
    xori  a0,a0,0x1      #invert least significant bit
    ret

/*
* ------------------------------------------------------------------------
* cs0_int_handler()
*
* Core Software Interrupt: this interrupt's flag is set by
* _tn_arch_context_switch_pend() when the kernel needs to perform
* context switch.
*
*/
cs0_int_handler:
    addi sp, sp, context_size

    #store all general purpose registers to the stack
    sw x1, 0(sp)
    sw x3, 4(sp)
    sw x4, 8(sp)
    sw x5, 12(sp)
    sw x6, 16(sp)
    sw x7, 20(sp)
    sw x8, 24(sp)
    sw x9, 28(sp)
    sw x10, 32(sp)
    sw x11, 36(sp)
    sw x12, 40(sp)
    sw x13, 44(sp)
    sw x14, 48(sp)
    sw x15, 52(sp)
    sw x16, 56(sp)
    sw x17, 60(sp)
    sw x18, 64(sp)
    sw x19, 68(sp)
    sw x20, 72(sp)
    sw x21, 76(sp)
    sw x22, 80(sp)
    sw x23, 84(sp)
    sw x24, 88(sp)
    sw x25, 92(sp)
    sw x26, 96(sp)
    sw x27, 100(sp)
    sw x28, 104(sp)
    sw x29, 108(sp)
    sw x30, 112(sp)
    sw x31, 116(sp)
    # extract tasks pc from epc 
    csrrc x31, 65, zero # read epc without changing epc
    sw x31, 120(sp) # store pc to resume exec from

    # start switching tasks
    la x1, _tn_curr_run_task        # x1 = &_tn_curr_run_task
    lw a0, 0(x1)                    # a0 = _tn_curr_run_task
    sw sp, 0(a0)                    # store sp in preempted task's TCB

    lw x2, _tn_next_task_to_run     # x2 = _tn_next_task_to_run
    add a1, x2, zero                # a1 = _tn_next_task_to_run
    lw sp, 0(x2)                    # load sp of task to run

#if _TN_ON_CONTEXT_SWITCH_HANDLER
    /* 
    * arguments are set:
    * - a0: _tn_curr_run_task
    * - a1: _tn_next_task_to_run
    */
   jal     _tn_sys_on_context_switch
#endif
    
    sw      $x2, 0($x1)            /* _tn_curr_run_task = _tn_next_task_to_run */
    # TODO: check if there are bits to disable after handling interrupt

tn_restore_context:
    sw x31, 120(sp) # load epc from proocess stack
    csrrc zero, 65, x31 # write epc and discard values read

    lw x1, 0(sp)
    lw x3, 4(sp)
    lw x4, 8(sp)
    lw x5, 12(sp)
    lw x6, 16(sp)
    lw x7, 20(sp)
    lw x8, 24(sp)
    lw x9, 28(sp)
    lw x10, 32(sp)
    lw x11, 36(sp)
    lw x12, 40(sp)
    lw x13, 44(sp)
    lw x14, 48(sp)
    lw x15, 52(sp)
    lw x16, 56(sp)
    lw x17, 60(sp)
    lw x18, 64(sp)
    lw x19, 68(sp)
    lw x20, 72(sp)
    lw x21, 76(sp)
    lw x22, 80(sp)
    lw x23, 84(sp)
    lw x24, 88(sp)
    lw x25, 92(sp)
    lw x26, 96(sp)
    lw x27, 100(sp)
    lw x28, 104(sp)
    lw x29, 108(sp)
    lw x30, 112(sp)
    lw x31, 116(sp)
    
    addi sp, sp, -context_size # pop all elements used by context switching

    uret

/*
* ------------------------------------------------------------------------
* _tn_arch_context_switch_pend()
*
* Called whenever we need to switch context from one task to another.
* interrupts running task to perform context switch
* See comments in tn_arch.h
*/
_tn_arch_context_switch_pend:
    # todo: verify actual behavior, might need to use specific interrupt code
    # rather than zero for system interrupt code
    add a0, zero, zero
    ecall   # perform system interrupt for context switching

/**
* Should return 1 if <i>system ISR</i> is currently running, 0 otherwise.
*
* Refer to the section \ref interrupt_types for details on what is <i>system
* ISR</i>.
* we are returning zero always since
* risc v rv32 does not support nested interrupts
* therefore 
*/
_tn_arch_inside_isr:
# do we have ustatus ?
     add a0, zero, zero
     ret

/* disables all interrupts and 
returns previous content of uie register(status register)
* 
*/
_tn_arch_sr_save_int_dis:
    csrrw a0, 0x4, zero
    ret

/* restores status register(uie register)
to the provided value
*/
_tn_arch_sr_restore:
    csrrw zero, 0x4, a0
    ret

/*
 * See comments in the file `tn_arch.h`
 */
tn_arch_sched_dis_save:
    csrrci a0, 0x4, 0x2 # read timer bit and clear to disable scheduler
    andi a0, a0, 0x2 # make sure to mask unnessary bits
    ret

/*
 * See comments in the file `tn_arch.h`
 */
tn_arch_sched_restore:
    csrrs zero, 0x4, a0 # restore timer status to given state
    ret                 