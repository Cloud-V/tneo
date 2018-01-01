/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

 #include "_tn_tasks.h"
 #include "_tn_sys.h"

/*******************************************************************************
*    EXTERNAL DATA
******************************************************************************/
 
 //-- gp: provided by linker
//  extern unsigned long _gp;

/*******************************************************************************
 *    PROTECTED DATA
 ******************************************************************************/

/*
 * For comments on these variables, see the file tn_arch_risc_rv32.h
 */
void *tn_rv32_user_sp;
void *tn_rv32_int_sp;

/** used as replacement for software interrupts
 * rather than performing a low priority software interrupt
 * to perform context switch, we use this variable instead.
 * the timer interrupt handler must check it with every interrupt
 * and set it to zero before performing a context switch.
 */
char should_context_switch = 0;

// tn_arch 
void tn_arch_enable_timer();

/*******************************************************************************
 *    IMPLEMENTATION
 ******************************************************************************/

 void _tn_arch_sys_start(
    TN_UWord      *int_stack,
    TN_UWord       int_stack_size
    )
{ //-- set interrupt's top of the stack
    tn_rv32_int_sp = int_stack + int_stack_size/*'full desc stack' model*/;


    // enable interrupts
    tn_arch_int_en(); 
    //-- perform first context switch

    //enable timer
    tn_arch_enable_timer();

    _tn_arch_context_switch_now_nosave();
}

/*
 * See comments in the file `tn_arch.h`
 * please refer to cs0_int_handler for appropriate 
    location for the register values in the stack
 */
 TN_UWord *_tn_arch_stack_init(
    TN_TaskBody   *task_func,
    TN_UWord      *stack_low_addr,
    TN_UWord      *stack_high_addr,
    void          *param
    )
{
    TN_UWord *cur_stack_pt = stack_high_addr;
    /**  the size of the stack is 'context_size' mentioned in 
    * file 'tn_arch_risc_rv32.s' divided by register size + 1 since we 
        need to point to boundary location rather than the last element 
        in the stack
    **/ 
    cur_stack_pt -= 33; // 132 bytes we need for context saving
    for(int i = 0; i < 33;i++){
        cur_stack_pt[i] = 0;
    }
    /**setting the value of ra register to address of task exit
     * so we can exit the task after completing it's execution
    **/
    *cur_stack_pt = (TN_UWord)_tn_task_exit_nodelete; 
    //-- UEPC: address that PC is set to when context switch is done
    //   and `eret` is executed.
    //   We should set it to task body function address
    *(cur_stack_pt + 31) = (TN_UWord)task_func;
    //-- a0 - task's function argument 
    *(cur_stack_pt + 9) = (TN_UWord)param; 
    // put value provided by linker for global pointer
    

    _TN_UNUSED(stack_low_addr);
    return cur_stack_pt;
}