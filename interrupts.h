#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "types.h"
#include "port.h"
#include "gdt.h"

class InterruptManager;
//建立一个基类，以供其他外设
class InterruptHandler{
public:
    virtual uint32_t HandlerInterrupt(uint32_t esp);//获取方法
protected:
    InterruptHandler(uint8_t interruptNumber, InterruptManager* interruptManager);
    ~InterruptHandler();

    uint8_t interruptNumber;
    InterruptManager* interruptManager;
};

//管理中断
class InterruptManager{
friend class InterruptHandler;
public:
    InterruptManager(uint16_t hardwareInterruptOffset, GlobalDescriptorTable* gdt);
    ~InterruptManager();
    
    uint16_t HardwareInterruptOffset();
    void Activate();
    void Deactivate();

protected:

    InterruptHandler* handlers[256];
    static InterruptManager* ActiveInterruptManager;    //判断InterruptManager是否被多次创建

    /*实现IDT*/ 
    /*利用多态实现任务门 中断门 陷阱门描述符*/
    struct GateDescriptor
    {
        uint16_t handlerAdeerLowBits;//低位
        uint16_t gdt_codeSegmentSelector;//段选择符
        uint8_t reserved;//保留位
        uint8_t access; //访问控制
        uint16_t handlerAdeerHightBits; //高位
    }__attribute__((packed));//__attribute__((packed))：告诉编译器取消结构在编译过程总的优化对齐
    
    static GateDescriptor interruptDescriptorTable[256];

    //描述子  告诉系统IDT表在哪  GDT表用的是数组 这次用的是结构体
    //32位存放基地址 16位存放限长度
    struct InterruptDescriptorTablePointer
    {
        uint16_t size;
        uint32_t base;
        
    }__attribute__((packed));
    

    //获取入口地址
    static void SetInerruptDescriptorTableEntry(
        //传入的参数
        uint8_t interruptNumber,
        uint16_t codeSegmentSelectorOffset,
        void (*handler)(),   //处理函数的函数指针
        uint8_t DescriptorPrivilegeLevel, //级别
        uint8_t DescriptorType//类型
    );

     //处理中断请求。每个中断都有自己的编号
    //实现20个异常中断，17个外部中断
    static uint32_t HandleInterrupt(uint8_t interruptNumber, uint32_t esp); //只能访问静态成员
    uint32_t DohandleInterrupt(uint8_t interruptNumber, uint32_t esp);//非静态

    //内外部中断的interruptNumber是一致的  但其实不是一个位 外部会有一个偏移  为0x20
    uint16_t hardwareInterruptOffset;

    static void InterruptIgnore();

    //异常中断请求
    static void HandleException0x00();
    static void HandleException0x01();
    static void HandleException0x02();
    static void HandleException0x03();
    static void HandleException0x04();
    static void HandleException0x05();
    static void HandleException0x06();
    static void HandleException0x07();
    static void HandleException0x08();
    static void HandleException0x09();
    static void HandleException0x0A();
    static void HandleException0x0B();
    static void HandleException0x0C();
    static void HandleException0x0D();
    static void HandleException0x0E();
    static void HandleException0x0F();
    static void HandleException0x10();
    static void HandleException0x11();
    static void HandleException0x12();
    static void HandleException0x13();

    //外部中断请求
    static void HandleInterruptRequest0x00(); 
    static void HandleInterruptRequest0x01();
    static void HandleInterruptRequest0x02();
    static void HandleInterruptRequest0x03();
    static void HandleInterruptRequest0x04();
    static void HandleInterruptRequest0x05();
    static void HandleInterruptRequest0x06();
    static void HandleInterruptRequest0x07();
    static void HandleInterruptRequest0x08();
    static void HandleInterruptRequest0x09();
    static void HandleInterruptRequest0x0A();
    static void HandleInterruptRequest0x0B();
    static void HandleInterruptRequest0x0C();
    static void HandleInterruptRequest0x0D();
    static void HandleInterruptRequest0x0E();
    static void HandleInterruptRequest0x0F();
    static void HandleInterruptRequest0x31();


    //为了处理16个中断 要两个
    Port8BitSlow picMasterCommand;
    Port8BitSlow picMasterDate;
    Port8BitSlow picSlaveCommand;
    Port8BitSlow picSlaveDate;
    
};

#endif