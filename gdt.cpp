//函数实现文件
#include"gdt.h"
//构造函数
GlobalDescriptorTable::GlobalDescriptorTable()
    :nullSegmentDescriptor(0, 0, 0),
    unusedSegmentDescriptor(0, 0, 0),
    codeSegmentDescriptor(0, 64 * 1024 * 1024, 0x9a), //0x9a 查看GDT描述符格式  跟寄存器一样的用法
    dataSegmentDescriptor(0, 64 * 1024 * 1024, 0x92){
        uint32_t i[2];//数组 存放GDT描述子  ---告诉操作系统GDT的表在哪
        i[1] = (uint32_t)this; //this指针 --地址
        i[0] = sizeof(GlobalDescriptorTable) << 16; //偏移16位 即2个字节   小端：低位地址存放低位数据
        //c++汇编 内嵌汇编语法 __asm__(汇编语句模板：输出部分：输入部分：破坏描述部分) ---各部分用：分开
        //volatile:向GCC声明不允许对该内联汇编优化
        //%0是占位符  p是输入部分---一种操作，用于操作地址
        //通过lgdt访问i偏移2个字节的地址 无输出
        asm volatile("lgdt (%0)" : : "p"(((uint8_t *)i) + 2));
    }

//析构
GlobalDescriptorTable::~GlobalDescriptorTable(){}

uint16_t GlobalDescriptorTable::DataSegmentSelector(){
    //数据段地址-this地址 得到偏移多少 偏移为uint8_t类型
    return ((uint8_t*)&dataSegmentDescriptor - (uint8_t*)this) << 3;
}

uint16_t GlobalDescriptorTable::CodeSegmentSelector(){
    return ((uint8_t*)&codeSegmentDescriptor - (uint8_t*)this) << 3;
}
//limit 最大寻址能力限制
GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t type){
    uint8_t* target = (uint8_t*)this;
    //当limit小于1048576 则用不到保护模式
    //为了向前兼容，所以设定为1048576=2^20
    if(limit < 1048576){
        target[6] = 0x40;
    }else{
        //判断后16位是否全1，当全1时会越界，得用保护模式
        if((limit & 0xfff) != 0xfff){
            limit = (limit >> 12) - 1; //
        }else{
            limit = limit >> 12;
        }
        target[6] = 0xC0; //保护模式
   }

   target[0] = limit & 0xff;
   target[1] = (limit >> 8) & 0xff;
   target[6] |= (limit >> 16) & 0xf;

   target[2] = base & 0xff;
   target[3] = (base >> 8) & 0xff;
   target[4] = (base >> 16) & 0xff;
   target[7] = (base >> 24) & 0xff;

   target[5] = type;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Base(){
    uint8_t* target = (uint8_t*)this;
    uint32_t result = target[7];
    result = (result << 8) + target[4];
    result = (result << 8) + target[3];
    result = (result << 8) + target[2];
    return result;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::Limit(){
    uint8_t* target = (uint8_t*)this;
    uint32_t result = target[6] & 0xf;
    result = (result << 8) + target[1];
    result = (result << 8) + target[0];

    if((target[6] & 0xC0) == 0xC0)
        result = (result << 12) | 0xfff;
    return result;
}

