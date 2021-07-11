#include"spim.h"

int reg[32];//0为空闲，其他为占用
int reg_num[32];//记录寄存器中的变量的u_int
char* reg_name[32];//记录名字，永远不变
int count=0;
extern int table_num;//判断一下t*是不是在table_num里面即可，小于它
FILE* F=NULL;
//数组需要放入内存
//对于a=b+c这种形式，右边但凡是t**，之后都不会用，直接释放
struct REMfp_sp* rem_head=NULL;
struct REMfp_sp* rem_now=NULL;


void start_spim(InterCode head,FILE* f){
    F=f;
    start_reg();
    output_before_dec(f);
    start_dec(head,f);
    output_after_dec(f);
    start_translate(head,f);
}

void start_reg(){
    for(int i=0;i<32;i++){
        reg[i]=0;
        reg_num[i]=-1;
    }
	reg_name[0]="$zero";//不能用=0
	reg_name[1]="$at";
    reg_name[26]="$k0";
	reg_name[27]="$k1";//给汇编器，也不能用
	reg_name[2]="$v0";//函数返回值
	reg_name[3]="$v1";//自己用
	reg_name[4]="$a0";
	reg_name[5]="$a1";
	reg_name[6]="$a2";
	reg_name[7]="$a3";//函数前4个参数
	reg_name[8]="$t0";
	reg_name[9]="$t1";
	reg_name[10]="$t2";
	reg_name[11]="$t3";
	reg_name[12]="$t4";
	reg_name[13]="$t5";
	reg_name[14]="$t6";
	reg_name[15]="$t7";
    reg_name[24]="$t8";
	reg_name[25]="$t9";//随便用->内存
	reg_name[16]="$s0";
	reg_name[17]="$s1";
	reg_name[18]="$s2";
	reg_name[19]="$s3";
	reg_name[20]="$s4";
	reg_name[21]="$s5";
	reg_name[22]="$s6";
	reg_name[23]="$s7";//随便用->栈
	reg_name[28]="$gp";
	reg_name[29]="$sp";//栈
	reg_name[30]="$fp";//帧
	reg_name[31]="$ra";//不要用，返回地址
}

void start_translate(InterCode head,FILE* f){
    InterCode temp=head;
    //给main函数生成一个记录栈中变量的东西
    rem_head=(struct REMfp_sp*)malloc(sizeof(struct REMfp_sp));
    rem_head->offset=NULL;
    rem_head->next=NULL;
    rem_head->now_offset=0;
    rem_now=rem_head;
    while(temp!=NULL){
        if(temp->kind==ILABEL){
            //printf("a start_translate ILABEL\n");
            fprintf(f,"label");
            fprintf(f,"%d ",temp->u.ulabel.op->u_int);
            fprintf(f,":\n");
        }
        else if(temp->kind==IFUNCTION){
            //printf("a start_translate IFUNCTION\n");
            fprintf(f,"%s",temp->u.ulabel.op->u_char);
            fprintf(f,":\n");
            start_function(f);
        }
        else if(temp->kind==ASSIGN){
            //printf("a start_translate ASSIGN\n");
            Operand t1=temp->u.uassign.op1;
            Operand t2=temp->u.uassign.op2;
            if(t1==NULL||t2==NULL);
            else if(t2->kind==ADDRESS){
                //printf("a 1.3\n");
                int namet1=get_reg(f,t1);
                int namet2=get_reg(f,t2);
                fprintf(f,"lw %s, 0(%s)\n",reg_name[namet1],reg_name[namet2]);
                free_reg(namet2);
            }
            else{
                //printf("a 1.4\n");
                int namet1=get_reg(f,t1);
                int namet2=get_reg(f,t2);
                fprintf(f,"move %s, %s\n",reg_name[namet1],reg_name[namet2]);
                free_reg(namet2);
            }
        }
        else if(temp->kind==ADD){
            //printf("a translate_print  ADD\n");
            Operand t1=temp->u.ubinop.result;
            Operand t2=temp->u.ubinop.op1;
            Operand t3=temp->u.ubinop.op2;
            if(t1==NULL);
            else{
                if(t3->kind==CONSTANT){
                    int namet1=get_reg(f,t1);
                    int namet2=get_reg(f,t2);
                    fprintf(f,"addi %s, %s, %d\n",reg_name[namet1],reg_name[namet2],t3->u_int);
                    free_reg(namet2);
                }
                else{
                    int namet1=get_reg(f,t1);
                    int namet2=get_reg(f,t2);
                    int namet3=get_reg(f,t3);
                    fprintf(f,"add %s, %s, %s\n",reg_name[namet1],reg_name[namet2],reg_name[namet3]);
                    free_reg(namet2);
                    free_reg(namet3);
                }
            }
        }
        else if(temp->kind==SUB){
            //printf("a translate_print SUB\n");
            Operand t1=temp->u.ubinop.result;
            Operand t2=temp->u.ubinop.op1;
            Operand t3=temp->u.ubinop.op2;
            if(t1==NULL);
            else{
                if(t3->kind==CONSTANT){
                    int namet1=get_reg(f,t1);
                    int namet2=get_reg(f,t2);
                    fprintf(f,"addi %s, %s, %d\n",reg_name[namet1],reg_name[namet2],-(t3->u_int));
                    free_reg(namet2);
                }
                else{
                    int namet1=get_reg(f,t1);
                    int namet2=get_reg(f,t2);
                    int namet3=get_reg(f,t3);
                    fprintf(f,"sub %s, %s, %s\n",reg_name[namet1],reg_name[namet2],reg_name[namet3]);
                    free_reg(namet2);
                    free_reg(namet3);
                }
            }
        }
        else if(temp->kind==MUL){
            //printf("a translate_print MUL\n");
            Operand t1=temp->u.ubinop.result;
            Operand t2=temp->u.ubinop.op1;
            Operand t3=temp->u.ubinop.op2;
            if(t1==NULL);
            else{
                    int namet1=get_reg(f,t1);
                    int namet2=get_reg(f,t2);
                    int namet3=get_reg(f,t3);
                    fprintf(f,"mul %s, %s, %s\n",reg_name[namet1],reg_name[namet2],reg_name[namet3]);
                    free_reg(namet2);
                    free_reg(namet3);
                }
        }
        else if(temp->kind==DIV){
            //printf("a translate_print DIV\n");
            Operand t1=temp->u.ubinop.result;
            Operand t2=temp->u.ubinop.op1;
            Operand t3=temp->u.ubinop.op2;
            if(t1==NULL);
            else{
                    int namet1=get_reg(f,t1);
                    int namet2=get_reg(f,t2);
                    int namet3=get_reg(f,t3);
                    fprintf(f,"div %s, %s\n",reg_name[namet2],reg_name[namet3]);
                    fprintf(f,"mflo %s\n",reg_name[namet1]);
                    free_reg(namet2);
                    free_reg(namet3);
                }
        }
        else if(temp->kind==ADDRASS2){
            //printf("a translate_print ADDRASS2\n");
            Operand t1=temp->u.uassign.op1;
            Operand t2=temp->u.uassign.op2;
            int namet1=get_reg(f,t1);
            int namet2=get_reg(f,t2);
            fprintf(f,"lw %s, 0(%s)\n",reg_name[namet1],reg_name[namet2]);
            free_reg(namet2);
        }
        else if(temp->kind==ADDRASS3){
            //printf("a translate_print ADDRASS3\n");
            Operand t1=temp->u.uassign.op1;
            Operand t2=temp->u.uassign.op2;
            int namet1=get_reg(f,t1);
            int namet2=get_reg(f,t2);
            fprintf(f,"sw %s, 0(%s)\n",reg_name[namet2],reg_name[namet1]);
            free_reg(namet2);
        }
        else if(temp->kind==ADDRASS1){
            //printf("a translate_print ADDRASS2\n");
            //只有可能是数组,在.data节声明过，直接用
            Operand t1=temp->u.uassign.op1;
            Operand t2=temp->u.uassign.op2;
            int namet1=get_reg(f,t1);
            int namet2=get_reg(f,t2);
            fprintf(f,"la %s, _t%d\n",reg_name[namet2],t2->u_int);
            fprintf(f,"move %s, %s\n",reg_name[namet1],reg_name[namet2]);
        }
        else if(temp->kind==GOTO){
            //printf("a translate_print GOTO\n");
            fprintf(f,"j label");
            fprintf(f,"%d\n",temp->u.ulabel.op->u_int);
        }
        else if(temp->kind==IF){
            //printf("a translate_print IF\n");
            Operand t1=temp->u.uif.x;
            Operand re=temp->u.uif.relop;
            Operand t2=temp->u.uif.y;
            Operand t3=temp->u.uif.z;
            int namet1=get_reg(f,t1);
            int namet2=get_reg(f,t2);
            if(t1->kind==ADDRESS) fprintf(f,"lw %s, 0(%s)\n",reg_name[namet1],reg_name[namet1]);
            if(t2->kind==ADDRESS) fprintf(f,"lw %s, 0(%s)\n",reg_name[namet2],reg_name[namet2]);

            if(re->u_int==0) fprintf(f,"beq");
            else if(re->u_int==1) fprintf(f,"bne");
            else if(re->u_int==2) fprintf(f,"blt");
            else if(re->u_int==3) fprintf(f,"bgt");
            else if(re->u_int==4) fprintf(f,"ble");
            else if(re->u_int==5) fprintf(f,"bge");

            fprintf(f," %s, %s,",reg_name[namet1],reg_name[namet2]);
            fprintf(f," label");
            fprintf(f,"%d\n",t3->u_int);
            free_reg(namet1);
            free_reg(namet2);
        }
        else if(temp->kind==RETURN){
            //printf("a translate_print RETURN\n");
            int namet1=get_reg(f,temp->u.ulabel.op);
            fprintf(f,"move $v0, %s\n",reg_name[namet1]);
            free_reg(namet1);
            return_function(f);
            //出函数后，将所有寄存器取出来

        }
        else if(temp->kind==READ){
            //printf("a translate_print READ\n");
            int namet1=get_reg(f,temp->u.ulabel.op);
            fprintf(f, "addi $sp, $sp, -4\n");
            fprintf(f, "sw $ra, 0($sp)\n");
            fprintf(f, "jal read\n");
            fprintf(f, "lw $ra, 0($sp)\n");
            fprintf(f, "addi $sp, $sp, 4\n");
            fprintf(f, "move %s, $v0\n",reg_name[namet1]);
        }
        else if(temp->kind==WRITE){
            //printf("a translate_print WRITE\n");
            int namet1=get_reg(f,temp->u.ulabel.op);
            fprintf(f, "move $a0, %s\n",reg_name[namet1]);
            fprintf(f, "addi $sp, $sp, -4\n");
            fprintf(f, "sw $ra, 0($sp)\n");
            fprintf(f, "jal write\n");
            fprintf(f, "lw $ra, 0($sp)\n");
            fprintf(f, "addi $sp, $sp, 4\n");
            free_reg(namet1);
        }
        else if(temp->kind==ARG){
            //printf("a translate_print ARG\n");
            //把一系列arg处理掉
            InterCode arg=temp;
            int arg_num=1;
            while(arg->next!=NULL&&arg->next->kind==ARG){
                arg=arg->next;
                arg_num++;
            }
            //传的是最后一个ARG
            function_with_Arg(f,arg,arg_num);
            temp=arg;
        }
        else if(temp->kind==CALL){
            //printf("a translate_print CALL\n");
            Operand t1=temp->u.uassign.op1;//返回值赋值
            Operand t2=temp->u.uassign.op2;//函数名
            //进入函数前，将所有寄存器封装好
            push_all_reg(f);
            int namet1=0;
            if(t1!=NULL){
                namet1=get_reg(f,t1);
                fprintf(f, "jal %s\n",t2->u_char);
                pop_all_reg(f);
                fprintf(f, "move %s, $v0\n",reg_name[namet1]);
            }
            else{
                Operand t0=new_temp();
                namet1=get_reg(f,t0);
                fprintf(f, "jal %s\n",t2->u_char);
                pop_all_reg(f);
                fprintf(f, "move %s, $v0\n",reg_name[namet1]);
                free_reg(namet1);
            }
        }
        else if(temp->kind==PARAM){
            InterCode param=temp;
            int param_num=1;
            while(param->next!=NULL&&param->next->kind==PARAM){
                param=param->next;
                param_num++;
            }
            //传的是第一个PARAM
            function_with_Param(f,temp,param_num);
            temp=param;
            //printf("a translate_print PARAM\n");
            // fprintf(f,"PARAM ");
            // if(temp->u.ulabel.op->kind==CONSTANT) fprintf(f,"#%d",temp->u.ulabel.op->u_int);
            // else fprintf(f,"t%d",temp->u.ulabel.op->u_int);
        }
        reg[8]=0;
        reg[9]=0;
        temp=temp->next;
    }
}

void push_all_reg(FILE* f){
    //将10-25的寄存器封装好
    fprintf(F, "addi $sp, $sp, -64\n");
    fprintf(F, "sw $t2, 0($sp)\n");
    fprintf(F, "sw $t3, 4($sp)\n");
    fprintf(F, "sw $t4, 8($sp)\n");
    fprintf(F, "sw $t5, 12($sp)\n");
    fprintf(F, "sw $t6, 16($sp)\n");
    fprintf(F, "sw $t7, 20($sp)\n");
    fprintf(F, "sw $t8, 24($sp)\n");
    fprintf(F, "sw $t9, 28($sp)\n");
    fprintf(F, "sw $s0, 32($sp)\n");
    fprintf(F, "sw $s1, 36($sp)\n");
    fprintf(F, "sw $s2, 40($sp)\n");
    fprintf(F, "sw $s3, 44($sp)\n");
    fprintf(F, "sw $s4, 48($sp)\n");
    fprintf(F, "sw $s5, 52($sp)\n");
    fprintf(F, "sw $s6, 56($sp)\n");
    fprintf(F, "sw $s7, 60($sp)\n");
}

void pop_all_reg(FILE* f){
    fprintf(F, "lw $t2, 0($sp)\n");
    fprintf(F, "lw $t3, 4($sp)\n");
    fprintf(F, "lw $t4, 8($sp)\n");
    fprintf(F, "lw $t5, 12($sp)\n");
    fprintf(F, "lw $t6, 16($sp)\n");
    fprintf(F, "lw $t7, 20($sp)\n");
    fprintf(F, "lw $t8, 24($sp)\n");
    fprintf(F, "lw $t9, 28($sp)\n");
    fprintf(F, "lw $s0, 32($sp)\n");
    fprintf(F, "lw $s1, 36($sp)\n");
    fprintf(F, "lw $s2, 40($sp)\n");
    fprintf(F, "lw $s3, 44($sp)\n");
    fprintf(F, "lw $s4, 48($sp)\n");
    fprintf(F, "lw $s5, 52($sp)\n");
    fprintf(F, "lw $s6, 56($sp)\n");
    fprintf(F, "lw $s7, 60($sp)\n");
    fprintf(F, "addi $sp, $sp, 64\n");
}


void free_reg(int now){
    if(now>9&&now<16){
        reg[now]=0;
        reg_num[now]=-1;
    }
}

int get_reg(FILE* f,Operand now){
    //保证给的都是寄存器，也就是寄存器->栈->寄存器
    //常数使用固定寄存器-8,9
    //10-15用于我自己定义的变量
    //16-24用于存储程序给出的变量（也就是在符号表中的变量）
    //25用于存储在栈中取出的变量
    //printf("a get_reg\n");
    if(now->kind==CONSTANT){
        if(reg[8]==0){
            fprintf(f,"li $t0, %d\n",now->u_int);
            reg[8]=1;
            return 8;
        } 
        else{
            fprintf(f,"li $t1, %d\n",now->u_int);
            reg[9]=1;
            return 9;
        }
    }
    else if(now->u_int<table_num){
        //首先找是否已经给这个变量寄存器
        //如果有，直接返回
        for(int i=16;i<25;i++){
            if(reg_num[i]==now->u_int){
                return i;
            }
        }
        //如果没有，首先看是否有空寄存器，有就放入
        //有空寄存器时，栈里是一定没有数据的
        for(int i=16;i<25;i++){
            if(reg[i]==0){
                reg[i]=1;
                reg_num[i]=now->u_int; 
                return i;
            }
        }
        //其他都使用t9这个寄存器，如果该寄存器中已经有值，就把它放入栈中
        //如果没有空寄存器，那么找是否已经给它一个栈中的偏移量
        struct REMoffset* temp=rem_now->offset;
        while(temp!=NULL){
            if(temp->var_num==now->u_int){
                if(reg[25]!=0) push_into(reg_num[25]);
                fprintf(f,"lw $t9, -%d($fp)\n",temp->offset);
                reg_num[25]=now->u_int;
                reg[25]=1;
                return 25;
            }
        }
        //如果都没有，那就说明该变量没有使用过
        //那么就要把t9中的变量压入栈，这个变量直接使用t9
        if(reg[25]!=0) push_into(reg_num[25]);
        reg_num[25]=now->u_int;
        reg[25]=1;
        return 25;
    }
    else{
        //是我自己用的变量，当它出现在等式右侧时，就可以把它释放
        //先找有没有使用过
        for(int i=10;i<=15;i++){
            if(reg_num[i]==now->u_int){
                return i;
            }
        }
        //没有使用过，就找一个空的给它
        //因为自己使用的变量，一定没有这么多
        //一定找的到空的
        for(int i=10;i<=15;i++){
            if(reg[i]==0){
                reg[i]=1;
                reg_num[i]=now->u_int;
                return i;
            }
        }
        return 10;
    }
}

void start_dec(InterCode head,FILE* f){
    InterCode temp=head;
    while(temp!=NULL){
        if(temp->kind==DEC){
            //肯定是一维数组，且为int/float
            //space好像更好用？
            //_t1: .space 40
            fprintf(f,"_t%d: .space ",temp->u.udec.op->u_int);
            fprintf(f,"%d\n",temp->u.udec.size);
        }
        temp=temp->next;
    }
}

void output_before_dec(FILE* f){
    fprintf(f,".data\n");
	fprintf(f,"_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(f,"_ret: .asciiz \"\\n\"\n");
    fprintf(f,".globl main\n");
}

void output_after_dec(FILE* f){
    fprintf(f,".text\n");
    //READ需要用到a0,v0,ra
	fprintf(f,"read:\n");
	fprintf(f," li $v0, 4\n");
	fprintf(f," la $a0, _prompt\n");
	fprintf(f," syscall\n");
	fprintf(f," li $v0, 5\n");
	fprintf(f," syscall\n");
	fprintf(f," jr $ra\n");
    fprintf(f,"\n");
    //WRITE需要用到v0,a0,ra
	fprintf(f,"write:\n");
	fprintf(f," li $v0, 1\n");
	fprintf(f," syscall\n");
	fprintf(f," li $v0, 4\n");
	fprintf(f," la $a0, _ret\n");
	fprintf(f," syscall\n");
	fprintf(f," move $v0, $0\n");
	fprintf(f," jr $ra\n");
    fprintf(f,"\n");
}

void function_with_Arg(FILE* f,InterCode arg,int num){
    //传递的是最后一个arg
    InterCode temp=arg;
    if(num>0){
        int namet1=get_reg(f,temp->u.ulabel.op);
        fprintf(f, "move $a0, %s\n",reg_name[namet1]);
        free_reg(namet1);
    }
    if(num>1){
        temp=temp->before;
        int namet1=get_reg(f,temp->u.ulabel.op);
        fprintf(f, "move $a1, %s\n",reg_name[namet1]);
        free_reg(namet1);
    }
    if(num>2){
        temp=temp->before;
        int namet1=get_reg(f,temp->u.ulabel.op);
        fprintf(f, "move $a2, %s\n",reg_name[namet1]);
        free_reg(namet1);
    }
    if(num>3){
        temp=temp->before;
        int namet1=get_reg(f,temp->u.ulabel.op);
        fprintf(f, "move $a3, %s\n",reg_name[namet1]);
        free_reg(namet1);
    }
    if(num>4){
        int numsub=num-4;
        int offset=numsub*4;
        fprintf(f, "addi $sp, $sp, -%d\n",offset);
        for(int i=0;i<numsub;i++){
            temp=temp->before;
            int namet1=get_reg(f,temp->u.ulabel.op);
            fprintf(f, "sw %s, %d($sp)\n",reg_name[namet1],i*4);
            free_reg(namet1);
        }
    }
}

void function_with_Param(FILE* f,InterCode arg,int num){
    //传递的是第一个param
    InterCode temp=arg;
    if(num>0){
        int namet1=get_reg(f,temp->u.ulabel.op);
        fprintf(f, "move %s, $a0\n",reg_name[namet1]);
    }
    if(num>1){
        temp=temp->next;
        int namet1=get_reg(f,temp->u.ulabel.op);
        fprintf(f, "move %s, $a1\n",reg_name[namet1]);
    }
    if(num>2){
        temp=temp->next;
        int namet1=get_reg(f,temp->u.ulabel.op);
        fprintf(f, "move %s, $a2\n",reg_name[namet1]);
    }
    if(num>3){
        temp=temp->next;
        int namet1=get_reg(f,temp->u.ulabel.op);
        fprintf(f, "move %s, $a3\n",reg_name[namet1]);
    }
    if(num>4){
        int numsub=num-4;
        for(int i=0;i<numsub;i++){
            temp=temp->next;
            int namet1=get_reg(f,temp->u.ulabel.op);
            fprintf(f, "lw %d($fp), %s\n",i*4+8,reg_name[namet1]);
        }
    }
}

//把t9中的数据放进栈中
void push_into(int now){
    struct REMoffset* temp=rem_now->offset;
    while(temp!=NULL){
        if(temp->var_num==now){
            return;
        }
    }
    temp=rem_now->offset;
    while(temp->next!=NULL) temp=temp->next;
    struct REMoffset* anew=(struct REMoffset*)malloc(sizeof(struct REMoffset));
    temp->next=anew;
    anew->next=NULL;
    rem_now->now_offset=rem_now->now_offset+4;
    anew->offset=rem_now->now_offset;
    anew->var_num=now;
    fprintf(F, "addi $sp, $sp, -4\n");
    fprintf(F, "sw $t9, -%d($fp)\n",anew->offset);
}

void start_function(FILE* f){
    //在每个函数开始之前存进去
	fprintf(f,"subu $sp, $sp, 8\n");
	fprintf(f,"sw $ra, 4($sp)\n");
	fprintf(f,"sw $fp, 0($sp)\n");
	fprintf(f,"move $fp, $sp\n");
}

void return_function(FILE* f){
    //在每个函数返回时恢复堆栈
    fprintf(f,"move $sp, $fp\n");
	fprintf(f,"lw $ra, 4($sp)\n");
	fprintf(f,"lw $fp, 0($sp)\n");
    fprintf(f,"addu $sp, $sp, 8\n");
	fprintf(f,"jr $ra\n");
}