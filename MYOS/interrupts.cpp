#include "interrupts.h"
//前置声明
void printf(const char*);

//自动注册
InterruptHandler::InterruptHandler(uint8_t interruptNumber,InterruptManager* interruptManager){
    this->interruptNumber = interruptNumber;
    this->interruptManager = interruptManager;
    interruptManager->handlers[interruptNumber] = this;
}

InterruptHandler::~InterruptHandler(){
    if(interruptManager ->handlers[interruptNumber] == this){
        interruptManager ->handlers[interruptNumber] = 0;
    }
}

uint32_t InterruptHandler::HandlerInterrupt(uint32_t esp){
    return esp;
}


InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

InterruptManager* InterruptManager::ActiveInterruptManager = 0;


void InterruptManager::SetInerruptDescriptorTableEntry(
        uint8_t interruptNumber,
        uint16_t codeSegmentSelectorOffset,
        void (*handler)(),
        uint8_t DescriptorPrivilegeLevel,
        uint8_t DescriptorType){
            const uint8_t IDT_DESC_PRESENT = 0x80; //固定为0x80
            //256个中断 1kb内
            interruptDescriptorTable[interruptNumber].handlerAdeerLowBits = ((uint32_t)handler) & 0xffff;
            interruptDescriptorTable[interruptNumber].handlerAdeerHightBits = ((uint32_t)handler >> 16) & 0xffff;
            interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
            interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | ((DescriptorPrivilegeLevel & 3) << 5) | DescriptorType;
            interruptDescriptorTable[interruptNumber].reserved = 0; 
        }
//:先初始化端口
InterruptManager::InterruptManager(uint16_t hardwareInterruptOffset, GlobalDescriptorTable* gdt)
    :picMasterCommand(0x20),
    picMasterDate(0x21),
    picSlaveCommand(0xA0),
    picSlaveDate(0xA1){
    this->hardwareInterruptOffset = hardwareInterruptOffset;
    uint16_t codeSegment = (gdt->CodeSegmentSelector()) >> 3;
    //中断门
    const uint8_t IDT_INTERRUPT_GATE = 0xe;
    for(uint16_t i = 0; i < 256; i++){
        //初始化为最高级中断门
        handlers[i] = 0;
        SetInerruptDescriptorTableEntry(i, codeSegment, &InterruptIgnore, 0, IDT_INTERRUPT_GATE);
    }

//处理特殊中断 20和17个
    SetInerruptDescriptorTableEntry(0x00, codeSegment, &HandleException0x00, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x01, codeSegment, &HandleException0x01, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x02, codeSegment, &HandleException0x02, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x03, codeSegment, &HandleException0x03, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x04, codeSegment, &HandleException0x04, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x05, codeSegment, &HandleException0x05, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x06, codeSegment, &HandleException0x06, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x07, codeSegment, &HandleException0x07, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x08, codeSegment, &HandleException0x08, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x09, codeSegment, &HandleException0x09, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x0A, codeSegment, &HandleException0x0A, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x0B, codeSegment, &HandleException0x0B, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x0C, codeSegment, &HandleException0x0C, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x0D, codeSegment, &HandleException0x0D, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x0E, codeSegment, &HandleException0x0E, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x0F, codeSegment, &HandleException0x0F, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x10, codeSegment, &HandleException0x10, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x11, codeSegment, &HandleException0x11, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x12, codeSegment, &HandleException0x12, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(0x13, codeSegment, &HandleException0x13, 0, IDT_INTERRUPT_GATE);


    SetInerruptDescriptorTableEntry(hardwareInterruptOffset + 0x00, codeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(hardwareInterruptOffset + 0x01, codeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(hardwareInterruptOffset + 0x02, codeSegment, &HandleInterruptRequest0x02, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(hardwareInterruptOffset + 0x03, codeSegment, &HandleInterruptRequest0x03, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(hardwareInterruptOffset + 0x04, codeSegment, &HandleInterruptRequest0x04, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(hardwareInterruptOffset + 0x05, codeSegment, &HandleInterruptRequest0x05, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(hardwareInterruptOffset + 0x06, codeSegment, &HandleInterruptRequest0x06, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(hardwareInterruptOffset + 0x07, codeSegment, &HandleInterruptRequest0x07, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(hardwareInterruptOffset + 0x08, codeSegment, &HandleInterruptRequest0x08, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(hardwareInterruptOffset + 0x09, codeSegment, &HandleInterruptRequest0x09, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(hardwareInterruptOffset + 0x0A, codeSegment, &HandleInterruptRequest0x0A, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(hardwareInterruptOffset + 0x0B, codeSegment, &HandleInterruptRequest0x0B, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(hardwareInterruptOffset + 0x0C, codeSegment, &HandleInterruptRequest0x0C, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(hardwareInterruptOffset + 0x0D, codeSegment, &HandleInterruptRequest0x0D, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(hardwareInterruptOffset + 0x0E, codeSegment, &HandleInterruptRequest0x0E, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(hardwareInterruptOffset + 0x0F, codeSegment, &HandleInterruptRequest0x0F, 0, IDT_INTERRUPT_GATE);
    SetInerruptDescriptorTableEntry(hardwareInterruptOffset + 0x31, codeSegment, &HandleInterruptRequest0x31, 0, IDT_INTERRUPT_GATE);

    ////规定这么写的

    picMasterCommand.Write(0x11);
    picSlaveCommand.Write(0x11);

    picMasterDate.Write(hardwareInterruptOffset);
    picSlaveDate.Write(hardwareInterruptOffset + 8);

    picMasterDate.Write(0x04);
    picSlaveDate.Write(0x02);

    picMasterDate.Write(0x01);
    picSlaveDate.Write(0x01);

    picMasterDate.Write(0x00); //不屏蔽任何中断 传入0x00
    picSlaveDate.Write(0x00);

    //初始化IDT表
    InterruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(GateDescriptor) - 1; //256个描述符
    idt.base = (uint32_t)interruptDescriptorTable;//表地址

    asm volatile("lidt %0" : :"m" (idt));
}
InterruptManager::~InterruptManager(){}

uint16_t InterruptManager:: HardwareInterruptOffset(){
    return hardwareInterruptOffset;
}

//activate 启动函数
void InterruptManager::Activate(){
    if(ActiveInterruptManager != 0){
        ActiveInterruptManager->Deactivate();
    }
    ActiveInterruptManager = this;
    asm("sti"); //开中断
}

//Deactivate 启动函数
void InterruptManager::Deactivate(){
    if(ActiveInterruptManager == this){
        ActiveInterruptManager= 0;
        asm("cli");
    }
}

//函数实现 esp：栈顶
uint32_t InterruptManager::HandleInterrupt(uint8_t interruptNumber, uint32_t esp){
    if(ActiveInterruptManager != 0){
        return ActiveInterruptManager->DohandleInterrupt(interruptNumber,esp);
    }
    return esp;
}
//
uint32_t InterruptManager::DohandleInterrupt(uint8_t interruptNumber, uint32_t esp){
    if(handlers[interruptNumber] != 0){
        esp = handlers[interruptNumber]->HandlerInterrupt(esp);
    }else if(interruptNumber != hardwareInterruptOffset) {//判断是否为时钟中断，是输出
        char* foo = (char*)"UNHANDLED INTERRUPT 0X00";
        const char* hex = "0123456789ABCDEF";
        foo[22] = hex[(interruptNumber >> 4) & 0x0f];
        foo[23] = hex[interruptNumber & 0x0f];
        printf((const char*)foo);
        } 

    if(hardwareInterruptOffset <= interruptNumber && interruptNumber < hardwareInterruptOffset + 16){
        picMasterCommand.Write(0x20);//写EOI 固定为0x20 结尾
        if(hardwareInterruptOffset + 8 <= interruptNumber){//判断主从
            picSlaveCommand.Write(0x20);
        }
    }
    return esp;
}

