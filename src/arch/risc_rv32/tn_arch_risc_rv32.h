/*******************************************************************************
 *
 * TNeo: real-time kernel initially based on TNKernel
 *
 *    TNKernel:                  copyright 2004, 2013 Yuri Tiomkin.
 *    PIC32-specific routines:   copyright 2013, 2014 Anders Montonen.
 *    TNeo:                      copyright 2014       Dmitry Frank.
 *
 *    TNeo was born as a thorough review and re-implementation of
 *    TNKernel. The new kernel has well-formed code, inherited bugs are fixed
 *    as well as new features being added, and it is tested carefully with
 *    unit-tests.
 *
 *    API is changed somewhat, so it's not 100% compatible with TNKernel,
 *    hence the new name: TNeo.
 *
 *    Permission to use, copy, modify, and distribute this software in source
 *    and binary forms and its documentation for any purpose and without fee
 *    is hereby granted, provided that the above copyright notice appear
 *    in all copies and that both that copyright notice and this permission
 *    notice appear in supporting documentation.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE DMITRY FRANK AND CONTRIBUTORS "AS IS"
 *    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DMITRY FRANK OR CONTRIBUTORS BE
 *    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *    THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/
/**
 *
 * \file
 *
 * RISC V RV32i architecture-dependent routines
 *
 */
#ifndef _TN_ARCH_RISC_RV32
#define _TN_ARCH_RISC_RV32

/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/
#include "../../core/tn_cfg_dispatch.h"

#ifdef __cplusplus
extern "C"  {     /*}*/
#endif


/**
 * FFS - find first set bit. Used in `_find_next_task_to_run()` function.
 * Say, for `0xa8` it should return `3`.
 *
 * May be not defined: in this case, naive algorithm will be used.
 */
#define  _TN_FFS(x) (32 - __builtin_clz((x) & (0 - (x))))

/**
 * Used by the kernel as a signal that something really bad happened.
 * Indicates TNeo bugs as well as illegal kernel usage, e.g. sleeping in
 * the idle task callback or build-time configuration mismatch (see
 * `#TN_CHECK_BUILD_CFG` for details on the last one)
 *
 * Typically, set to assembler instruction that causes debugger to halt.
 */
#define  _TN_FATAL_ERRORF(error_msg, ...)         \
   {__asm__ volatile("ebreak"); __asm__ volatile ("add zero, zero, zero");}




/**
 * Compiler-specific attribute that should be placed **before** declaration of
 * array used for stack. It is needed because there are often additional 
 * restrictions applied to alignment of stack, so, to meet them, stack arrays
 * need to be declared with these macros.
 *
 * @see TN_ARCH_STK_ATTR_AFTER
 */
#define TN_ARCH_STK_ATTR_BEFORE

/**
 * Compiler-specific attribute that should be placed **after** declaration of
 * array used for stack. It is needed because there are often additional 
 * restrictions applied to alignment of stack, so, to meet them, stack arrays
 * need to be declared with these macros.
 *
 * @see TN_ARCH_STK_ATTR_BEFORE
 */

#define TN_ARCH_STK_ATTR_AFTER      __attribute__((aligned(0x8)))

/**
 * Minimum task's stack size, in words, not in bytes; includes a space for
 * context plus for parameters passed to task's body function.
 */
#define  TN_MIN_STACK_SIZE          36

/**
 * Width of `int` type.
 */
#define  TN_INT_WIDTH               32

/**
 * Unsigned integer type whose size is equal to the size of CPU register.
 * Typically it's plain `unsigned int`.
 */
typedef  unsigned int               TN_UWord;

/**
 * Unsigned integer type that is able to store pointers.
 * We need it because some platforms don't define `uintptr_t`.
 * Typically it's `unsigned int`.
 */
typedef  unsigned int               TN_UIntPtr;

/**
 * Maximum number of priorities available, this value usually matches
 * `#TN_INT_WIDTH`.
 *
 * @see TN_PRIORITIES_CNT
 */
#define  TN_PRIORITIES_MAX_CNT      TN_INT_WIDTH

/**
 * Value for infinite waiting, usually matches `ULONG_MAX`,
 * because `#TN_TickCnt` is declared as `unsigned long`.
 */
#define  TN_WAIT_INFINITE           (TN_TickCnt)0xFFFFFFFF

/**
 * Value for initializing the unused space of task's stack
 */
#define  TN_FILL_STACK_VAL          0xFEEDFACE


/**
 * Variable name that is used for storing interrupts state
 * by macros TN_INTSAVE_DATA and friends
 */
#define TN_INTSAVE_VAR              tn_save_status_reg

/**
 * Declares variable that is used by macros `TN_INT_DIS_SAVE()` and
 * `TN_INT_RESTORE()` for storing status register value.
 *
 * @see `TN_INT_DIS_SAVE()`
 * @see `TN_INT_RESTORE()`
 */
#define  TN_INTSAVE_DATA     TN_UWord TN_INTSAVE_VAR = 0;

/**
 * The same as `#TN_INTSAVE_DATA` but for using in ISR together with
 * `TN_INT_IDIS_SAVE()`, `TN_INT_IRESTORE()`.
 *
 * @see `TN_INT_IDIS_SAVE()`
 * @see `TN_INT_IRESTORE()`
 */
#define  TN_INTSAVE_DATA_INT        TN_INTSAVE_DATA

/**
 * Disable interrupts and return previous value of status register,
 * atomically. Similar `tn_arch_sr_save_int_dis()`, but implemented
 * as a macro, so it is potentially faster.
 *
 * Uses `#TN_INTSAVE_DATA` as a temporary storage.
 *
 * @see `#TN_INTSAVE_DATA`
 * @see `tn_arch_sr_save_int_dis()`
 */
#define TN_INT_DIS_SAVE()        tn_save_status_reg = tn_arch_sr_save_int_dis()

/**
 * Restore previously saved status register.
 * Similar to `tn_arch_sr_restore()`, but implemented as a macro,
 * so it is potentially faster.
 *
 * Uses `#TN_INTSAVE_DATA` as a temporary storage.
 *
 * @see `#TN_INTSAVE_DATA`
 * @see `tn_arch_sr_save_int_dis()`
 */
#define TN_INT_RESTORE()         tn_arch_sr_restore(tn_save_status_reg)

/**
 * The same as `TN_INT_DIS_SAVE()` but for using in ISR.
 *
 * Uses `#TN_INTSAVE_DATA_INT` as a temporary storage.
 *
 * @see `#TN_INTSAVE_DATA_INT`
 */
#define TN_INT_IDIS_SAVE()       TN_INT_DIS_SAVE()

/**
 * The same as `TN_INT_RESTORE()` but for using in ISR.
 *
 * Uses `#TN_INTSAVE_DATA_INT` as a temporary storage.
 *
 * @see `#TN_INTSAVE_DATA_INT`
 */
#define TN_INT_IRESTORE()        TN_INT_RESTORE()

/**
 * Returns nonzero if interrupts are disabled, zero otherwise.
 */
#define TN_IS_INT_DISABLED()     (_tn_arch_is_int_disabled())

/**
 * Pend context switch from interrupt.
 */
#define _TN_CONTEXT_SWITCH_IPEND_IF_NEEDED()          \
   _tn_context_switch_pend_if_needed()


/**
 * Converts size in bytes to size in `#TN_UWord`.
 * For 32-bit platforms, we should shift it by 2 bit to the right;
 * for 16-bit platforms, we should shift it by 1 bit to the right.
 */
#define _TN_SIZE_BYTES_TO_UWORDS(size_in_bytes)    ((size_in_bytes) >> 2)

/**
 * If compiler does not conform to c99 standard, there's no inline keyword.
 * So, there's a special macro for that.
 */
#if TN_FORCED_INLINE
//TODO: if available, use some compiler-specific forced inline qualifier
#  define _TN_INLINE             inline
#else
#  define _TN_INLINE             inline
#endif

/**
 * For some compilers, order of these qualifiers matters
 * (at least when _TN_INLINE expands to some compiler-specific forced inline)
 */
#define _TN_STATIC_INLINE        static _TN_INLINE

/**
 * Sometimes compilers are buggy in high-optimization modes, and these
 * bugs are often could be worked around by adding the `volatile` keyword.
 *
 * It is compiler-dependent, so, there's a special macro for that.
 */
#define _TN_VOLATILE_WORKAROUND   /* nothing */


#define _TN_ARCH_STACK_PT_TYPE   _TN_ARCH_STACK_PT_TYPE__FULL
/** we will rely on descending stack 
 */
#define _TN_ARCH_STACK_DIR       _TN_ARCH_STACK_DIR__DESC

/** although the CPU does not support nesting
 * we need to know if we are inside any interrupts.
 * since we do not have any flags to indicate if we are inside
 * an ISR we can increment and decrement that counter with every ISR enter/exit respectively
 * this counter is mainly due to the fact that the kernel provides users with the option to check
 * whether they are in a system ISR or not
 */
// int interrupt_nest_count = 0;

/*
 * pointers to user task stack pointer and 
 * interrupt stack pointer
 * 
 */
void *tn_rv32_user_sp;
void *tn_rv32_int_sp;


#define tn_rv32_soft_isr(vec)                                                   \
__attribute__((__noinline__)) void _func##vec(void);                           \
void __attribute__((naked))                                              \
     _isr##vec(void)                                                           \
{                                                                              \
    asm volatile("addi sp, sp, -132");                                         \
    asm volatile("sw x1, 0(sp)");                                              \
    asm volatile("sw x3, 8(sp)");                                              \
    asm volatile("sw x4, 12(sp)");                                             \
    asm volatile("sw x5, 16(sp)");                                             \
    asm volatile("sw x6, 20(sp)");                                             \
    asm volatile("sw x7, 24(sp)");                                             \
    asm volatile("sw x8, 28(sp)");                                             \
    asm volatile("sw x9, 32(sp)");                                             \
    asm volatile("sw x10, 36(sp)");                                            \
    asm volatile("sw x11, 40(sp)");                                            \
    asm volatile("sw x12, 44(sp)");                                            \
    asm volatile("sw x13, 48(sp)");                                            \
    asm volatile("sw x14, 52(sp)");                                            \
    asm volatile("sw x15, 56(sp)");                                            \
    asm volatile("sw x16, 60(sp)");                                            \
    asm volatile("sw x17, 64(sp)");                                            \
    asm volatile("sw x18, 68(sp)");                                            \
    asm volatile("sw x19, 72(sp)");                                            \
    asm volatile("sw x20, 76(sp)");                                            \
    asm volatile("sw x21, 80(sp)");                                            \
    asm volatile("sw x22, 84(sp)");                                            \
    asm volatile("sw x23, 88(sp)");                                            \
    asm volatile("sw x24, 92(sp)");                                            \
    asm volatile("sw x25, 96(sp)");                                            \
    asm volatile("sw x26, 100(sp)");                                           \
    asm volatile("sw x27, 104(sp)");                                           \
    asm volatile("sw x28, 108(sp)");                                           \
    asm volatile("sw x29, 112(sp)");                                           \
    asm volatile("sw x30, 116(sp)");                                           \
    asm volatile("sw x31, 120(sp)");                                           \
                                                                               \
    asm volatile("la t0, tn_rv32_user_sp");                                    \
    asm volatile("sw sp, 0(t0)");                                              \
    asm volatile("lw sp, tn_rv32_int_sp");                                     \
                                                                               \
    /* call user provided usr  */                                              \
    /* NOTE: jumping using the immediate directly is possible because  */      \
    /* the memory range in soc is small enough to be expressed using   */      \
    /* the immediate given to jal. this will need to be changed if */          \
    /* if the ram size was to be increased */                                  \
     asm volatile("jal x1, _func"#vec);                                        \
                                                                               \
    /* return to user task stack   */                                          \
    asm volatile("lw sp, tn_rv32_user_sp");                                    \
                                                                               \
    /* restore context */                                                      \
    asm volatile("lw x1, 0(sp)");                                              \
    asm volatile("lw x3, 8(sp)");                                              \
    asm volatile("lw x4, 12(sp)");                                             \
    asm volatile("lw x5, 16(sp)");                                             \
    asm volatile("lw x6, 20(sp)");                                             \
    asm volatile("lw x7, 24(sp)");                                             \
    asm volatile("lw x8, 28(sp)");                                             \
    asm volatile("lw x9, 32(sp)");                                             \
    asm volatile("lw x10, 36(sp)");                                            \
    asm volatile("lw x11, 40(sp)");                                            \
    asm volatile("lw x12, 44(sp)");                                            \
    asm volatile("lw x13, 48(sp)");                                            \
    asm volatile("lw x14, 52(sp)");                                            \
    asm volatile("lw x15, 56(sp)");                                            \
    asm volatile("lw x16, 60(sp)");                                            \
    asm volatile("lw x17, 64(sp)");                                            \
    asm volatile("lw x18, 68(sp)");                                            \
    asm volatile("lw x19, 72(sp)");                                            \
    asm volatile("lw x20, 76(sp)");                                            \
    asm volatile("lw x21, 80(sp)");                                            \
    asm volatile("lw x22, 84(sp)");                                            \
    asm volatile("lw x23, 88(sp)");                                            \
    asm volatile("lw x24, 92(sp)");                                            \
    asm volatile("lw x25, 96(sp)");                                            \
    asm volatile("lw x26, 100(sp)");                                           \
    asm volatile("lw x27, 104(sp)");                                           \
    asm volatile("lw x28, 108(sp)");                                           \
    asm volatile("lw x29, 112(sp)");                                           \
    asm volatile("lw x30, 116(sp)");                                           \
    asm volatile("lw x31, 120(sp)");                                           \
                                                                               \
    asm volatile("addi sp, sp, 132");                                 \
    asm volatile("uret");                                                      \
}__attribute((__noinline__)) void _func##vec(void)



#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif