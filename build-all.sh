make TN_ARCH=rv32i TN_COMPILER=riscv64-unknown-elf-gcc

if [ $? -ne 0 ];then
    echo 'make failed'
    exit
fi

mv /home/hasebou/projects/tneo/bin/rv32i/riscv64-unknown-elf-gcc/tneo_rv32i_riscv64-unknown-elf-gcc.a \
    examples/basic/arch/risc_rv32/

if [ $? -ne 0 ];then
    echo 'mv failed'
    exit
fi

cd /home/hasebou/projects/tneo/examples/basic/arch/risc_rv32

if [ $? -ne 0 ];then
    echo 'cd failed'
    exit
fi

riscv64-unknown-elf-gcc -c main.c -I /home/hasebou/projects/tneo/src/  -D __RISC_V_RV32i__ -march=rv32i -mabi=ilp32 -nostartfiles -nostdlib

if [ $? -ne 0 ];then
    echo 'gcc failed'
    exit
fi

riscv64-unknown-elf-ld main.o tneo_rv32i_riscv64-unknown-elf-gcc.a -T link.ld -o main -melf32lriscv 

if [ $? -ne 0 ];then
    echo 'linker failed'
    exit
fi

riscv64-unknown-elf-objdump -d main > assembly
riscv64-unknown-elf-objcopy -O binary main main.bin

if [ $? -ne 0 ];then
    echo 'objdump or objcopy failed'
    exit
fi

rm -rf /home/hasebou/projects/tneo/_obj/