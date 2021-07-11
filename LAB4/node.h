#ifndef NODE_H
#define NODE_H


struct Node{
    struct Node* child;//孩子节点
    struct Node* brother;//右侧兄弟节点
    int linenumber;//记录行号，用于打印
    char name[30];//记录该单元规定的名字(token)
    int judge;//judge=0为词法单元，judge=1为语法单元，用于输出判断
    union {
        char char_name[30];
        int int_number;
        float float_number;
    };//记录该单元附加信息int-float-idname,int/float’number
    //语法单元由空串生成的，记录其int_number=0,其余的记为1
};

#endif
//printf("Error type 17 at Line %d: Undefined structure \"%s\".\n",fir_bro->linenumber,fir_bro->child->char_name);
//FieldList struct_field=(FieldList)malloc(sizeof(struct FieldList_));
//Type return_type=(Type)malloc(sizeof(struct Type_));
//struct FUNCTION* func=(struct FUNCTION*)malloc(sizeof(struct FUNCTION));
//Operand func_ope=(Operand)malloc(sizeof(struct Operand_));
//InterCode func_in=(InterCode)malloc(sizeof(struct InterCode_));
//(struct REMfp_sp*)malloc(sizeof(struct REMfp_sp));
//(struct REMoffset*)malloc(sizeof(struct REMoffset));