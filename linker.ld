ENTRY(loader)   /*指定程序入口地址*/
OUTPUT_FORMAT(elf32-i386)/*输出elf文件*/
OUTPUT_ARCH(i386:i386)

SECTIONS /*链接脚本的主体*/
{
    . = 0X0100000;

    .text :
    {
        *(.multiboot)
        *(.text*)
        *(.rodata)
    }

    .data :
    {
        _start_ctors = .;
        /*KEEP---告诉编译器，这部分不要被垃圾回收*/
        KEEP(*(.init_array)); /*init_array:带构造函数的全局对象生成时放在init_array段里*/
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)));
        _end_ctors = .;

        *(.data)
    }

    .bss :
    {
        *(.bss)
    }

    /DISCARD/ :
    {
        *(.fini_array*)
        *(.comment)
    }
}