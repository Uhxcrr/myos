#ifndef __GDT_H
#define __GDT_H
#include"types.h"

//定义GDT
class GlobalDescriptorTable{
public:
    //定义描述符
    class SegmentDescriptor{
    public:
        SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t type);
        uint32_t Base();//返回
        uint32_t Limit();
    private:
        uint16_t limit_lo;
        uint16_t base_lo;
        uint8_t base_hi;
        uint8_t type;
        uint8_t flags_limit_hi;
        uint8_t base_vhi;
    }__attribute__((packed)); // __attribute__((packed))：告诉编译器取消结构在编译过程总的优化对齐

    //定义段操作符
    SegmentDescriptor nullSegmentDescriptor; 
    SegmentDescriptor unusedSegmentDescriptor;
    SegmentDescriptor codeSegmentDescriptor;
    SegmentDescriptor dataSegmentDescriptor;

    //定义构造函数
public:
    GlobalDescriptorTable();
    ~GlobalDescriptorTable();

    uint16_t CodeSegmentSelector();//获取代码段和数据段
    uint16_t DataSegmentSelector();

};
#endif