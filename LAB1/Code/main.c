#include<stdio.h>
#include<stdlib.h>

extern FILE* yyin;
extern int yylineno;
extern struct Node* head;
extern int is_error;
int yylex();


int main(int argc,char** argv){
    if(argc<=1)
        return 1;
    FILE* f=fopen(argv[1],"r");
    if(!f)
    {
        perror(argv[1]);
        return 1;
    }
    yylineno=1;
    yyrestart(f);
    yyparse();
    //printf("head: %u\n",head);
    if(is_error==0){
        print_tree(head);}
    return 0;
}

//lexic中不能使用注释，会错误
//syntax中可以使用注释

/*TODO:
注释！1.3

*/