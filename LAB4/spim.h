#ifndef SPIM_H
#define SPIM_H

#include"interim.h"
//记录每一个压进栈中的变量相对于fp的偏移量
struct REMoffset{
    int offset;//记录相对于fp的偏移量
    int var_num;//变量的t1
    struct REMoffset* next;
};
//记录每一个函数第一个压入栈中的变量
//只记录压倒栈中的变量，寄存器的变量给寄存器记录
struct REMfp_sp{
    struct REMoffset* offset;
    struct REMfp_sp* next;
    int now_offset;
};


void start_spim(InterCode head,FILE* F);
void start_reg();
void start_translate(InterCode head,FILE* f);
void start_dec(InterCode head,FILE* f);
void output_before_dec(FILE* f);
//等式左边传0，等式右边传1
//右边的可释放
void free_reg(int now);
int get_reg(FILE* f,Operand now);
void output_after_dec(FILE* f);
void function_with_Arg(FILE* f,InterCode arg,int num);
void function_with_Param(FILE* f,InterCode arg,int num);
void push_into(int now);
void start_function(FILE* f);
void return_function(FILE* f);

void push_all_reg(FILE* f);
void pop_all_reg(FILE* f);

#endif