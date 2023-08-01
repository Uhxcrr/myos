#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"
#include "mouse.h"

void printf(const char* str){
    static uint16_t* VideoMemory = (uint16_t*)0xb8000; //获取屏幕首位
    static uint8_t x = 0, y = 0;//横纵坐标

    for(int i = 0; str[i]; i++){
        switch (str[i])
        {
        case '\n':
            y++;
            x = 0;
            break;
        default:
            VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | str[i];
            x++;
            break;
        }

        //屏幕横坐标满了
        if(x >= 80){
            x = 0;
            y++;
        }
        //屏幕纵坐标满了
        if(y >= 25){
            for(y = 0; y < 25; y++){
                for(x = 0; x < 80; x++){
                    VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xff00) | ' ';
                }
            }
            x = 0;
            y = 0;
        }
    }
}
typedef void(*constructor)();//定义的函数指针
//构造一些东西
extern "C" constructor start_ctors; //全局声明 start_ctors  end_ctors
extern "C" constructor end_ctors;
//实现调用constructor函数
extern "C" void callConstructors(){
    //遍历从star_ctors到end_ctors之间的init_arry构造,从头构造一遍
    for(constructor* i = &start_ctors; i != &end_ctors; i++){
        (*i)(); //调用
    }
}

extern "C" void kernelMain(void* multiboot_structure, uint32_t magicnumber){
    printf("hello world\n");
    printf("cc");

    GlobalDescriptorTable gdt;//创建gdt表
    InterruptManager interrupts(0x20, &gdt);//调用中断
    KeyBoardDriver keyboard(&interrupts);//初始化
    MouseDriver mouse(&interrupts);//初始化
    interrupts.Activate();//调取
    while (1);
}