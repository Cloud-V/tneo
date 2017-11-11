ENTRY(_start)


MEMORY
{
    ram :
        ORIGIN = 0x00000000,
        LENGTH = 20K
    stack :
        ORIGIN = 20K
        LENGTH = 6K
}

SECTIONS
 {
/*
 . = 0x10;
 .timer_vector :
 {
    *(.timer_vector)
 }
*/
 /* text: test code section */
  . = 0x0;
 .text :
 {
     *tn_arch_risc_rv32.o(.text);
    *(.text)
 } > ram

 /* data: Initialized data segment */
 .data :
 {
    *(.data)
 } > ram

 /* Uintialized data */
  .bss :
  {
    *(.bss)
  } > ram
 /* End of uninitalized data segement */
 _end = .;
 }

PROVIDE(_endram = ORIGIN(stack));
PROVIDE(_fstack = ORIGIN(stack) + LENGTH(stack) - 4);
