#ifndef _TN_ARCH_RISC_RV32
#define _TN_ARCH_RISC_RV32

/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/
#include "../../core/tn_cfg_dispatch.h"

#ifdef __cplusplus
extern "C"  {     /*}*/
#endif


/*******************************************************************************
 *    PROTECTED GLOBAL DATA
 ******************************************************************************/

/// saved task stack pointer. Needed when switching stack pointer from 
/// task stack to interrupt stack.
extern void *tn_rv32_user_sp;

/// saved ISR stack pointer. Needed when switching stack pointer from
/// interrupt stack to task stack.
extern void *tn_rv32_int_sp;











#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif