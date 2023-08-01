.set MAGIC, 0x1badb002 /*GRUB魔术块  必须等于0x1badb002*/
.set FLAGS, (1 << 0 | 1 << 1) /*GRUB标志块--简单设置为0*/
.set CHECKSUM, -(MAGIC + FLAGS) /*校验块  三者数值总和为0 */ 
/*.code32  //;重定义文件是32位的  机器是64位的  不加这段后面push 会报错*/

.section .multboot
	.long MAGIC
	.long FLAGS
	.long CHECKSUM

.section .text
/*调用外部函数*/
.extern _kernelMain
.extern _callConstructors

/*让链接器能看到*/
.global loader

loader:
    mov $kernel_stack, %esp   /*esp为寄存器 将kernel_stack地址mov 到esp寄存器中*/

    call _callConstructors /*调用函数 进行初始化*/

    push %eax /*eax 32位寄存器 范围0-0xFFFFFFFF 压入栈堆 $是取地址 而不是取值*/
    push %ebx
    call _kernelMain
_stop:
    cli /*这个命令没有任何操作数，直接使用即可
		使用之后标志寄存器的IF位会置0，表示屏蔽其他可屏蔽中断*/
    hlt /*CPU会卡在这个位置，直到有中断发生，低功耗*/
    jmp _stop /*jmp无条件转跳*/
    
/*.text:以下是代码段
.data:以下是初始化数据段
.bss:以下是未初始化数据段*/
.section .bss
.space 2*1024*1024
kernel_stack:
