MEMORY
{
    ram :
        ORIGIN = 0x00000000,
        LENGTH = 4096
}

SECTIONS {
    . = 0x10; /* interrupt vectors start from 16 byte*/
    
    .ecall_vector ALIGN (0x10) : 
    {
        *(.ecall_vector)
    }

    .ebreak_vector ALIGN (0x10) : 
    {
        *(.ebreak_vector)
    }

    .timer_vector ALIGN (0x10) : 
    {
        *(.timer_vector)
    }

    .extern_int_vector ALIGN (0x10) : 
    {
        *(.extern_int_vector)
    }
    . = 0x0050;          /* the code should be loaded at interrupt handler */

    .text ALIGN (0x1000) :   /* align at 4 KB */
    {
        *(.text)             /* all text sections from all files */
    }

    .rodata ALIGN (0x1000) : /* align at 4 KB */
    {
        *(.rodata*)          /* all read-only data sections from all files */
    }

    .data ALIGN (0x1000) :   /* align at 4 KB */
    {
        *(.data)             /* all data sections from all files */
    }

    .bss ALIGN (0x1000) :    /* align at 4 KB */
    {
        *(COMMON)            /* all COMMON sections from all files */
        *(.bss*)              /* all bss sections from all files */
    }
}
