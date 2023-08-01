#ifndef __MOUSE_H
#define __MOUSE_H

#include "types.h"
#include "interrupts.h"
#include "port.h"

class MouseDriver : public InterruptHandler{
public:
    MouseDriver(InterruptManager* manager);
    ~MouseDriver();
    virtual uint32_t HandleInterrupt(uint32_t esp);
private:
    Port8Bit dataport;
    Port8Bit commandport;
 //鼠标读取的是一个数据流，有三字节和四字节两种，这我们用三字节的模式
    uint8_t buffer[3];//第一个字节包含xy，第二第三个字节包含xy移动的距离
    uint8_t offset;//表示当前是哪个字节的信息
    uint8_t buttons;//按键按下/释放状态

    int8_t x,y;//鼠标初始位置 要无整形  否则会越界
};


#endif