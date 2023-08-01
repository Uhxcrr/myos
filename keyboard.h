#ifndef __KEY_BOARD_H
#define __KEY_BOARD_H

#include "types.h"
#include "interrupts.h"
#include "port.h"

//继承中断中的基类
class KeyBoardDriver : public InterruptHandler{
public:
    KeyBoardDriver(InterruptManager* manager);
    ~KeyBoardDriver();
    //处理函数
    virtual uint32_t HandleInterrupt(uint32_t esp); //传入栈帧

private:
    Port8Bit dataport;  //数据端口
    Port8Bit commandport; //命令端口
};

#endif