TNeo: a real-time kernel (RTOS)
==============

TNeo is a compact and fast real-time kernel for embedded 32/16 bits
microprocessors. It performs a preemptive priority-based scheduling and a
round-robin scheduling for the tasks with identical priority.

You might want to read an article: [How I ended up writing new real-time kernel](http://dmitryfrank.com/articles/how_i_ended_up_writing_my_own_kernel).

TNeo was born as a thorough review and re-implementation of [TNKernel](http://tnkernel.com) v2.7. The new kernel has well-formed code, inherited bugs are fixed as well as new features being added, it is well documented and tested carefully with unit-tests.

Currently it is available for the following architectures:

- ARM Cortex-M cores: Cortex-M0/M0+/M1/M3/M4/M4F *(supported toolchains: GCC,
  Keil RealView, clang, IAR)*
- Microchip: PIC32/PIC24/dsPIC
- RISC-V Dwarf RV32

Comprehensive documentation is available in two forms: html and pdf.

  * Latest stable TNeo: [html](https://dfrank.bitbucket.io/tneokernel_api/latest/html/), [pdf](https://dfrank.bitbucket.io/tneokernel_api/latest/latex/tneo.pdf)
  * Current development TNeo BETA: [html](https://dfrank.bitbucket.io/tneokernel_api/dev/html/), [pdf](https://dfrank.bitbucket.io/tneokernel_api/dev/latex/tneo.pdf)
  * [Changelog](https://dfrank.bitbucket.io/tneokernel_api/dev/html/changelog.html)
  

Index of all available docs can be found [here](https://dfrank.bitbucket.io/tneokernel_api/).

Documentation is generated by means of [doxygen](http://goo.gl/RQHRYr).

-----------------------------------------------------------------------------

# Sample code

```cpp

//   (job for which task was created at all)
const int size = 150;
int TEST_ARR[150]; // array to sort


struct TN_Task task_a = {};

void task_a_body(void *par)
{
    // fill array in descending order
    for(int i = 0; i < size;i++){
        TEST_ARR[i] = size - i;
    }
    
    // bubble sort in ascending order
    int i, j;
    for (i = 0; i < size-1; i++){
        for (j = 0; j < size-i-1; j++){
            if (TEST_ARR[j] > TEST_ARR[j+1]){
                int temp = TEST_ARR[j];
                TEST_ARR[j] = TEST_ARR[j+1];
                TEST_ARR[j+1] = temp;
            }
        } 
    }

    while(1);
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



int main{

/*
call to disable interrupts before calling the kernel.
this allow kernel to start correctly
*/
tn_arch_int_dis();

/*
To register an interrupt handler use the following macro.
The first argument is the ISR handler name and the second is the interrupt number from 0 to 15
*/
tn_rv32_soft_isr(test_one, 0);

/*
set the rate at which the kernel runs
*/
void setTimerPeriod(100000);

/* 
start the kernel by providing the stacks used by the interrupt method and idle task.
These are statically allocated as global variables.
*/
tn_sys_start(
      idle_task_stack,
      IDLE_TASK_STACK_SIZE,
      interrupt_stack,
      INTERRUPT_STACK_SIZE,
      init_task_create,
      NULL
      );

}
```