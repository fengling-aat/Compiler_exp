%{
#include<stdio.h>
#include<string.h>
extern int yylineno;
extern char* yytext;
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
#define YYSTYPE struct Node* 
struct Node* head=NULL;//每当生成一个新malloc，就更新一次，因为最后的头一定最后生成
int is_error=0;//0表示没有错误，打印；其余++
struct Node* create_Node(struct Node* c,char* name,int line)
{
    struct Node* newnode=(struct Node*)malloc(sizeof(struct Node));
    newnode->judge=1;
    newnode->child=c;
    newnode->brother=NULL;
    newnode->linenumber=line;
    newnode->int_number=1;
    strcpy(newnode->name,name);
    head=newnode;
    ("head: %u\n",head);
    ("newnode'name: %s\n",head->name);
    return newnode;
}
void print_node(struct Node* now){
    if(now->judge==0){
        //词法
        if(!strcmp(now->name,"ID\0")){
            printf("ID: %s\n",now->char_name);}
        else if(!strcmp(now->name,"TYPE\0")){
            printf("TYPE: %s\n",now->char_name);}
        else if(!strcmp(now->name,"INT\0")){
            printf("INT: %u\n",now->int_number);}
        else if(!strcmp(now->name,"FLOAT\0")){
            printf("FLOAT: %f\n",now->float_number);}
        else{
            printf("%s\n",now->name);}}
    else{
        //语法
        printf("%s (%d)\n",now->name,now->linenumber);}
}

void print_tree(struct Node* heads){
    struct Node* tree_sort[2000];
    tree_sort[0]=heads;
    print_node(heads);
    int number=1;
    int depth=0;
    while(number!=0)
    {
        if(tree_sort[number-1]->child!=NULL){
            tree_sort[number]=tree_sort[number-1]->child;
            tree_sort[number-1]->child=NULL;
            number++;
            depth++;
            if(tree_sort[number-1]->judge!=0&&tree_sort[number-1]->int_number==0);
            else{
                for(int i=0;i<depth;i++){
                    printf("  ");}
                print_node(tree_sort[number-1]);
            }
        }
        else if(tree_sort[number-1]->brother!=NULL){
            tree_sort[number-1]=tree_sort[number-1]->brother;
            if(tree_sort[number-1]->judge!=0&&tree_sort[number-1]->int_number==0);
            else{
                for(int i=0;i<depth;i++){
                    printf("  ");}
                print_node(tree_sort[number-1]);
            }
        }
        else{
            number--;
            depth--;
        }
    }
}
//将所有token类型都记为node即可
//1.因为它们全都可以以node形式打印
//2.直接可以使用$1的方式进行取值
%}
%locations
%token STRUCT
%token RETURN
%token IF
%token ELSE
%token WHILE
%token SEMI
%token COMMA
%token ASSIGNOP
%token PLUS
%token MINUS
%token STAR
%token DIV
%token AND
%token OR
%token NOT
%token LP
%token RP
%token LB
%token RB
%token LC
%token RC
%token TYPE
%token INT
%token FLOAT
%token ID
%token RELOP
%token DOT
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NAGATE NOT
%right DOT LP LB RP RB
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%
Program:ExtDefList{
    //printf("Program->ExtDefList\n");
    if($1->int_number==0){
        $$=create_Node($1,"Program\0",$1->linenumber);
    }
    else
        $$=create_Node($1,"Program\0",@1.first_line);
    }

ExtDefList: {
    $$=create_Node(NULL,"ExtDefList\0",yylineno);
    $$->int_number=0;}
|ExtDef ExtDefList{
    //printf("ExtDefList->ExtDef ExtDefList\n");
    $$=create_Node($1,"ExtDefList\0",@1.first_line);
    $$->int_number=2;
    $1->brother=$2;}

ExtDef:Specifier ExtDecList SEMI{
    //printf("ExtDef->Specifier ExtDecList SEMI\n");
    $$=create_Node($1,"ExtDef\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}
|Specifier SEMI{
    //printf("ExtDef->Specifier SEMI\n");
    $$=create_Node($1,"ExtDef\0",@1.first_line);
    $1->brother=$2;}
|Specifier FunDec CompSt{
    //printf("ExtDef->Specifier FunDec CompSt\n");
    $$=create_Node($1,"ExtDef\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}
|error SEMI{
    //printf("error1\n");
}

ExtDecList:VarDec{
    //printf("ExtDecList->VarDec\n");
    $$=create_Node($1,"ExtDecList\0",@1.first_line);}
|VarDec COMMA ExtDecList{
    //printf("ExtDecList->VarDec COMMA ExtDecList\n");
    $$=create_Node($1,"ExtDecList\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}

Specifier:TYPE{
    //printf("Specifier->TYPE\n");
    $$=create_Node($1,"Specifier\0",@1.first_line);}
|StructSpecifier{
    //printf("Specifier->StructSpecifier\n");
    $$=create_Node($1,"Specifier\0",@1.first_line);}

StructSpecifier:STRUCT OptTag LC DefList RC{
    //printf("StructSpecifier->STRUCT OptTag LC DefList RC\n");
    $$=create_Node($1,"StructSpecifier\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;
    $3->brother=$4;
    $4->brother=$5;}
|STRUCT Tag{
    //printf("StructSpecifier->STRUCT Tag\n");
    $$=create_Node($1,"StructSpecifier\0",@1.first_line);
    $1->brother=$2;}
|STRUCT OptTag LC error RC{
    //printf("error2\n");
}
|STRUCT error LC DefList RC{
    //printf("error3\n");
}

OptTag: {
    $$=create_Node(NULL,"OptTag\0",0);
    $$->int_number=0;}
|ID{
    //printf("OptTag->ID\n");
    $$=create_Node($1,"OptTag\0",@1.first_line);}

Tag:ID{
    //printf("Tag->ID\n");
    $$=create_Node($1,"Tag\0",@1.first_line);}

VarDec:ID{
    //printf("VarDec->ID\n");
    $$=create_Node($1,"VarDec\0",@1.first_line);}
|VarDec LB INT RB{
    //printf("VarDec->VarDec LB INT RB\n");
    $$=create_Node($1,"VarDec\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;
    $3->brother=$4;}

FunDec:ID LP VarList RP{
    //printf("FunDec->ID LP VarList RP\n");
    $$=create_Node($1,"FunDec\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;
    $3->brother=$4;}
|ID LP RP{
    //printf("FunDec->ID LP RP\n");
    $$=create_Node($1,"FunDec\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}

VarList:ParamDec COMMA VarList{
    //printf("VarList->ParamDec COMMA VarList\n");
    $$=create_Node($1,"VarList\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}
|ParamDec{
    //printf("VarList->ParamDec\n");
    $$=create_Node($1,"VarList\0",@1.first_line);}

ParamDec:Specifier VarDec{
    //printf("ParamDec->Specifier VarDec\n");
    $$=create_Node($1,"ParamDec\0",@1.first_line);
    $1->brother=$2;}

CompSt:LC DefList StmtList RC{
    //printf("CompSt->LC DefList StmtList RC\n");
    $$=create_Node($1,"CompSt\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;
    $3->brother=$4;}
|error RC{
    //printf("error5\n");
    }

StmtList: {
    $$=create_Node(NULL,"StmtList\0",0);
    $$->int_number=0;}
|Stmt StmtList{
    //printf("StmtList->Stmt StmtList\n");
    $$=create_Node($1,"StmtList\0",@1.first_line);
    $1->brother=$2;}

Stmt:Exp SEMI{
    //printf("Stmt->Exp SEMI\n");
    $$=create_Node($1,"Stmt\0",@1.first_line);
    $1->brother=$2;}
|CompSt{
    //printf("Stmt->CompSt\n");
    $$=create_Node($1,"Stmt\0",@1.first_line);}
|RETURN Exp SEMI{
    //printf("Stmt->RETURN Exp SEMI\n");
    $$=create_Node($1,"Stmt\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}
|IF LP Exp RP Stmt %prec LOWER_THAN_ELSE{
    //printf("Stmt->IF LP Exp RP Stmt\n");
    $$=create_Node($1,"Stmt\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;
    $3->brother=$4;
    $4->brother=$5;}
|IF LP Exp RP Stmt ELSE Stmt{
    //printf("Stmt->IF LP Exp RP Stmt ELSE Stmt\n");
    $$=create_Node($1,"Stmt\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;
    $3->brother=$4;
    $4->brother=$5;
    $5->brother=$6;
    $6->brother=$7;}
|WHILE LP Exp RP Stmt{
    //printf("Stmt->WHILE LP Exp RP Stmt\n");
    $$=create_Node($1,"Stmt\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;
    $3->brother=$4;
    $4->brother=$5;}
|error SEMI{
    //printf("error6\n");
    }
|RETURN error SEMI{
    //printf("error7\n");
    }

DefList: {
    $$=create_Node(NULL,"DefList\0",0);
    $$->int_number=0;}
|Def DefList{
    //printf("DefList->Def DefList\n");
    $$=create_Node($1,"DefList\0",@1.first_line);
    $1->brother=$2;}

Def:Specifier DecList SEMI{
    //printf("Def->Specifier DecList SEMI\n");
    $$=create_Node($1,"Def\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}
|Specifier error SEMI{
    //printf("error8\n");
    }

DecList:Dec{
    //printf("DecList->Dec\n");
    $$=create_Node($1,"DecList\0",$1->linenumber);}
|Dec COMMA DecList{
    //printf("DecList->Dec COMMA DecList\n");
    $$=create_Node($1,"DecList\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}

Dec:VarDec{
    //printf("Dec->VarDec\n");
    $$=create_Node($1,"Dec\0",@1.first_line);}
|VarDec ASSIGNOP Exp{
    //printf("Dec->VarDec ASSIGNOP Exp\n");
    $$=create_Node($1,"Dec\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}

Exp:Exp ASSIGNOP Exp{
    //printf("Exp->Exp ASSIGNOP Exp\n");
    $$=create_Node($1,"Exp\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}
|Exp AND Exp{
    //printf("Exp->Exp AND Exp\n");
    $$=create_Node($1,"Exp\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}
|Exp OR Exp{
    //printf("Exp->Exp OR Exp\n");
    $$=create_Node($1,"Exp\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}
|Exp RELOP Exp{
    //printf("Exp->Exp RELOP Exp\n");
    $$=create_Node($1,"Exp\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}
|Exp PLUS Exp{
    //printf("Exp->Exp PLUS Exp\n");
    $$=create_Node($1,"Exp\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}
|Exp MINUS Exp{
    //printf("Exp->Exp MINUS Exp\n");
    $$=create_Node($1,"Exp\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}
|Exp STAR Exp{
    //printf("Exp->Exp STAR Exp\n");
    $$=create_Node($1,"Exp\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}
|Exp DIV Exp{
    //printf("Exp->Exp DIV Exp\n");
    $$=create_Node($1,"Exp\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}
|LP Exp RP{
    //printf("Exp->LP Exp RP\n");
    $$=create_Node($1,"Exp\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}
|MINUS Exp %prec NAGATE{
    //printf("Exp->MINUS Exp\n");
    $$=create_Node($1,"Exp\0",@1.first_line);
    $1->brother=$2;}
|NOT Exp{
    //printf("Exp->NOT Exp\n");
    $$=create_Node($1,"Exp\0",@1.first_line);
    $1->brother=$2;}
|ID LP Args RP{
    //printf("Exp->ID LP Args RP\n");
    $$=create_Node($1,"Exp\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;
    $3->brother=$4;}
|ID LP RP{
    //printf("Exp->ID LP RP\n");
    $$=create_Node($1,"Exp\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}
|Exp LB Exp RB{
    //printf("Exp->Exp LB Exp RB\n");
    $$=create_Node($1,"Exp\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;
    $3->brother=$4;}
|Exp DOT ID{
    //printf("Exp->Exp DOT ID\n");
    $$=create_Node($1,"Exp\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}
|ID{
    //printf("Exp->ID\n");
    $$=create_Node($1,"Exp\0",@1.first_line);}
|INT{
    //printf("Exp->INT\n");
    $$=create_Node($1,"Exp\0",@1.first_line);}
|FLOAT{
    //printf("Exp->FLOAT\n");
    $$=create_Node($1,"Exp\0",@1.first_line);}
|error RB{
    //printf("error9\n");
    }

Args:Exp COMMA Args{
    //printf("Args->Exp COMMA Args\n");
    $$=create_Node($1,"Args\0",@1.first_line);
    $1->brother=$2;
    $2->brother=$3;}
|Exp{
    //printf("Args->Exp\n");
    $$=create_Node($1,"Args\0",@1.first_line);}  
%%
#include"lex.yy.c"
yyerror(char* msg)
{
    //printf("Error type B at Line %d: syntax error.", yylineno);
    printf("Error type B at Line %d: syntax error at \"%s\".\n", yylineno, yytext);
    is_error++;
}
// yyprint(NODE* head);