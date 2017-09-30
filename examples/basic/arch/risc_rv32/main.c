#include "tn.h"
#include <string.h>
#include "tn_app_check.c"

//-- idle task stack size, in words
#define IDLE_TASK_STACK_SIZE          (TN_MIN_STACK_SIZE + 32)

//-- interrupt stack size, in words
#define INTERRUPT_STACK_SIZE          (TN_MIN_STACK_SIZE + 64)

//-- stack sizes of user tasks
#define TASK_A_STK_SIZE    (TN_MIN_STACK_SIZE + 96)
#define TASK_B_STK_SIZE    (TN_MIN_STACK_SIZE + 96)
#define TASK_C_STK_SIZE    (TN_MIN_STACK_SIZE + 96)

//-- user task priorities
#define TASK_A_PRIORITY    7
#define TASK_B_PRIORITY    6
#define TASK_C_PRIORITY    5

/*******************************************************************************
 *    DATA
 ******************************************************************************/

//-- Allocate arrays for stacks: stack for idle task
//   and for interrupts are the requirement of the kernel;
//   others are application-dependent.
//
//   We use convenience macro TN_STACK_ARR_DEF() for that.

TN_STACK_ARR_DEF(idle_task_stack, IDLE_TASK_STACK_SIZE);
TN_STACK_ARR_DEF(interrupt_stack, INTERRUPT_STACK_SIZE);

TN_STACK_ARR_DEF(task_a_stack, TASK_A_STK_SIZE);
TN_STACK_ARR_DEF(task_b_stack, TASK_B_STK_SIZE);
TN_STACK_ARR_DEF(task_c_stack, TASK_C_STK_SIZE);



//-- task structures

struct TN_Task task_a = {};
struct TN_Task task_b = {};
struct TN_Task task_c = {};

void appl_init(void);

void task_a_body(void *par)
{
   //-- this is a first created application task, so it needs to perform
   //   all the application initialization.
   appl_init();

   //-- and then, let's get to the primary job of the task
   //   (job for which task was created at all)
   int hello = 0;
   for(;;)
   {
      hello += 1;
      tn_task_sleep(500);
   }
}

void task_b_body(void *par)
{
    int hello2 = 0;
    for(;;)
    {
        hello2 += 2;
        tn_task_sleep(1000);
    }
}

void task_c_body(void *par)
{
    int hello3 = 0;
    for(;;)
    {
        hello3 += 3;
        tn_task_sleep(1500);
    }
}

void appl_init(void)
{
    tn_task_create(
        &task_b,
        task_b_body,
        TASK_B_PRIORITY,
        task_b_stack,
        TASK_B_STK_SIZE,
        NULL,
        (TN_TASK_CREATE_OPT_START)
        );

  tn_task_create(
        &task_c,
        task_c_body,
        TASK_C_PRIORITY,
        task_c_stack,
        TASK_C_STK_SIZE,
        NULL,
        (TN_TASK_CREATE_OPT_START)
        );
}

//-- idle callback that is called periodically from idle task
void idle_task_callback(void)
{

}

//-- create first application task(s)
void init_task_create(void)
{
   //-- task A performs complete application initialization,
   //   it's the first created application task
   tn_task_create(
         &task_a,                   //-- task structure
         task_a_body,               //-- task body function
         TASK_A_PRIORITY,           //-- task priority
         task_a_stack,              //-- task stack
         TASK_A_STK_SIZE,           //-- task stack size (in words)
         NULL,                      //-- task function parameter
         TN_TASK_CREATE_OPT_START   //-- creation option
         );
}

int main(void)
{
   //-- unconditionally disable interrupts
   tn_arch_int_dis();

   //-- call to tn_sys_start() never returns
   tn_sys_start(
         idle_task_stack,
         IDLE_TASK_STACK_SIZE,
         interrupt_stack,
         INTERRUPT_STACK_SIZE,
         init_task_create,
         NULL
         );
   return 1;
}
