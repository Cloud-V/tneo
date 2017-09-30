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
    cur_stack_pt -= 33;
    
    /**setting the value of ra register to address of task exit
     * so we can exit the task after completing it's execution
    **/
    *cur_stack_pt = (TN_UWord)_tn_task_exit_nodelete; 
    //-- UEPC: address that PC is set to when context switch is done
    //   and `eret` is executed.
    //   We should set it to task body function address
    *(cur_stack_pt + 30) = (TN_UWord)task_func;
    //-- a0 - task's function argument 
    *(cur_stack_pt + 8) = (TN_UWord)param; 
    // put value provided by linker for global pointer
    // *(cur_stack_pt + 1) = (TN_UWord)&_gp; 
    

    _TN_UNUSED(stack_low_addr);
    return cur_stack_pt;
}