#include "mouse.h"

MouseDriver::MouseDriver(InterruptManager* manager)
    :InterruptHandler(0x01 + manager->HardwareInterruptOffset(), manager),
    dataport(0x60),
    commandport(0x64),
    offset(0),
    buttons(0),
    x(40),
    y(12){
        uint16_t* VideoMemory = (uint16_t*)0xb8000;
        //字符显示包含两个字节  前1个字节表示颜色（前4比特是前景色，后4比特是背景色），后1个字节显示字符
        //反转
        VideoMemory[y * 80 + x] = ((VideoMemory[y*80 + x] & 0xf000) >> 4)
                                | ((VideoMemory[y*80 + x] & 0x0f00) << 4)
                                | (VideoMemory[y*80 + x] & 0x00ff);

        commandport.Write(0xa8); //使能
        commandport.Write(0x20);
        uint8_t status = (dataport.Read() | 2) & ~0x20;
        commandport.Write(0x60);
        dataport.Write(status);

        commandport.Write(0xd4);//往鼠标里写
        dataport.Write(0xf4);

        dataport.Read();//开始读取

    }

MouseDriver::~MouseDriver(){}

void printf(const char*);

uint32_t MouseDriver::HandleInterrupt(uint32_t esp){
    uint8_t status = commandport.Read();
    //判断是鼠标还是键盘
    if(!(status & 0x20)) return esp;

    buffer[offset] = dataport.Read();
    offset = (offset + 1) % 3;
    //offset==0，有xy移动的数据
    if(offset == 0){
        uint16_t* VideoMemory = (uint16_t*)0xb8000;
        VideoMemory[y * 80 + x] = ((VideoMemory[y*80 + x] & 0xf000) >> 4)
                                | ((VideoMemory[y*80 + x] & 0x0f00) << 4)
                                | (VideoMemory[y*80 + x] & 0x00ff);
        x += buffer[1];
        if(x < 0) x = 0;
        else if(x >= 80) x = 79;

        y -= buffer[2];
        if(y < 0) y = 0;
        else if(y >=25) y = 24;

        VideoMemory[y * 80 + x] = ((VideoMemory[y*80 + x] & 0xf000) >> 4)
                                | ((VideoMemory[y*80 + x] & 0x0f00) << 4)
                                | (VideoMemory[y*80 + x] & 0x00ff);   

        //按键按下变白
        for(uint8_t i = 0; i < 3; i++){
            if((buffer[0] & (1 << i)) != buttons & (1 << i)){
                 VideoMemory[y * 80 + x] = ((VideoMemory[y*80 + x] & 0xf000) >> 4)
                                         | ((VideoMemory[y*80 + x] & 0x0f00) << 4)
                                         | (VideoMemory[y*80 + x] & 0x00ff); 
            } 
        }  

        buttons = buffer[0];   
    }

    
    
    return esp;
}