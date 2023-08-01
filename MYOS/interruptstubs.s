.set IRQ_BASE, 0x20  /*定义宏*/
/*代码段*/
.section .text
/*调用外部函数*/
.extern __ZN16InterruptManager15HandleInterruptEhj

/*定义宏， 就不用一个个查看每个函数的名称*/
/*num 为变量*/
.macro HandleInterruptRequest num
/*用num代替0x* 用法为\变量\ ()是隔开EV 不然会将\EV识别为变量*/
.global __ZN16InterruptManager26HandleInterruptRequest\num\()Ev
__ZN16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber) /*movb  1byte*/
    jmp int_bottom
.endm /*结束宏*/

/*同样定义宏*/
.macro HandleException num
.global __ZN16InterruptManager19HandleException\num\()Ev
__ZN16InterruptManager19HandleException\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x02
HandleInterruptRequest 0x03
HandleInterruptRequest 0x04
HandleInterruptRequest 0x05
HandleInterruptRequest 0x06
HandleInterruptRequest 0x07
HandleInterruptRequest 0x08
HandleInterruptRequest 0x09
HandleInterruptRequest 0x0A
HandleInterruptRequest 0x0B
HandleInterruptRequest 0x0C
HandleInterruptRequest 0x0D
HandleInterruptRequest 0x0E
HandleInterruptRequest 0x0F
HandleInterruptRequest 0x31

HandleException 0x00
HandleException 0x01
HandleException 0x02
HandleException 0x03
HandleException 0x04
HandleException 0x05
HandleException 0x06
HandleException 0x07
HandleException 0x08
HandleException 0x09
HandleException 0x0A
HandleException 0x0B
HandleException 0x0C
HandleException 0x0D
HandleException 0x0E
HandleException 0x0F
HandleException 0x10
HandleException 0x11
HandleException 0x12
HandleException 0x13



/*给标签，要执行的代码--压栈*/
int_bottom:
    pusha /*pusha:把通用寄存器压栈，寄存器的入栈顺序依次是：EAX,ECX,EDX,EBX,ESP(初始值)，EBP,ESI,EDI*/
    pushl %ds  /*pushl：压入4byte*/
    pushl %es  /*ds这些都是寄存器，%是格式*/
    pushl %fs
    pushl %gs

    pushl %esp
    /*调用函数前 先压栈*/
    push (interruptnumber)
    call __ZN16InterruptManager15HandleInterruptEhj /*调用函数*/

    /*返回值给esp*/
    movl %eax, %esp
    /*出栈*/
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

    /*ignore函数*/
.global __ZN16InterruptManager15InterruptIgnoreEv
__ZN16InterruptManager15InterruptIgnoreEv:


    iret /*中断返回，表示中断已经执行完了*/

/*数据段*/
.data
    interruptnumber: .byte 0 /*初始化，数据段要初始化，bss段不用*/
