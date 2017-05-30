#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int line=1;//ȫ�ֱ��� �����˵ڼ���
FILE *C0_code;//ȫ�ֱ�����Դ�����ļ�����
enum symbol{becames,add,minus,times,slash,less,lessorequal,more,moreorequal,notequal,equal,semicolon,constsym,intsym,ident,integer,comma
,floatsym,realnumber,charsym,character,unsignedinteger,leftbracket,rightbracket,leftparent,rightparent,leftcurlybracket,rightcurlybracket,
voidsym,mainsym,ifsym,elsesym,whilesym,switchsym,casesym,colon,scanfsym,printfsym,returnsym,string};
//ö�ٱ��������г��ֵĴʵ�����
enum symbol sym;//���ʶ������ķ�������
char *symbol_rem[]={"becames","add","minus","times","slash","less","lessorequal","more","moreorequal","notequal","equal","semicolon","constsym","intsym","ident","integer","comma"
,"floatsym","realnumber","charsym","character","unsignedinteger","leftbracket","rightbracket","leftparent","rightparent","leftcurlybracket","rightcurlybracket",
"voidsym","mainsym","ifsym","elsesym","whilesym","switchsym","casesym","colon","scanfsym","printfsym","returnsym","string"};
int integernum;//���ʶ�����������
double realnum;//���ʶ�������ʵ��
char id[20];//���ʶ������ı�ʶ��
char token[100];//�ʷ�������ʱ������
char ch;//���������ַ�
char zf;//����������ַ�
char zfc[100];//����������ַ���
struct func{
    int num;//��������
    int parameter[10];//�������� 0 int 1 float 2 char
    int big;//��������ο�ʼ����
    int funcpoint;
};
union Valunion{
    int intval;
    char charval;
    double floatval;
    struct func funcval;
};
struct atab{
    char name[20];//��ʶ������
	int lev;//�ֲ���������ȫ�ֱ���,lev==0��Ϊȫ�ֱ���������Ϊ�ֲ�����
	int type;//��ʶ������,�������� int float char,�ֱ�Ϊ 0 1 2 ��3Ϊvoid
	int degree;//���鳤��
	int kind;//���� ���� ���������� 0 1 2
	union Valunion val;//����ֵ������
	int offset;//����ĵ�ַƫ����
	int is_array;
}tab[500];//�ݶ����ű���500��
int tabtop;//���ű�ָ��

char hc[50][100];//���������
int hc_top;//���������ָ��


enum p_code_name{ADD,LIT,SUB,MUL,DIV,LOD,LODT,LOA,STT,JPC,JMP,INT,STI,JMB,STO,QUT,LOG,LOGT,rei,ref,rec,wr,ws};//pcode���������
char *P_code_name_name[]={"ADD","LIT","SUB","MUL","DIV","LOD","LODT","LOA","STT","JPC","JMP","INT","STI","JMB","STO","QUT","LOG","LOGT","rei","ref","rec","wr","ws"};
/*
    LIT 0 A,��������A����ջ��
    ADD 0 0,ջ�����ջ����ӣ�������ڴ�ջ��
    SUB 0 0,��ջ����ջ����������ڴ�ջ��
    MUL 0 0,��ջ����ջ���ˣ�������ڴ�ջ��
    DIV 0 0,��ջ������ջ����������ڴ�ջ��
    LOD l A,�����Ϊl����Ե�ַΪA�ı�������ջ��
    LODT l 0,����ַΪջ���ı���ȡ������ջ��,���Ϊl
    LOA l A,�����Ϊl,��Ե�ַΪA�ĵ�ַȡ������ջ��
    STT l 0,��ջ�������ڴ�ջ����ַ��,���Ϊl
    LOG 0 A,�߼����㼯�ϣ����д�ջ����ջ�����߼����� ������ڴ�ջ��
            ˳������Ϊ1 > 2 >= 3 < 4 <= 5 != 6 ==
    LOGT 0 A�߼����㼯�ϣ����д�ջ����ջ�����߼����� �������ջ��
            ˳������Ϊ1 > 2 >= 3 < 4 <= 5 != 6 ==
    JPC 0 A,��ջ��Ϊ0 ����ת��ָ��A
    JMP 0 A,��������ת��A
    INT 0 A,�洢ջջ������A���洢��Ԫ
    STI l A,��������l������Ե�ַA��
    JMB 0 0,���������ret addr��
    STO l A,��ջ��Ԫ�ش��ڲ��Ϊl����Ե�ַΪA�Ĵ洢��
    QUT 0 0,�洢ջ��ջһ��
    rei 0 0
    ref 0 0
    rec 0 0��ȡָ��ӱ�׼���������ȡ,����ջ��
    wr 0 0��׼���ָ����ջ������׼���
*/
union p_par{//Ŀ�����������
    int zheng;
    float shi;
};
struct p_code_list{
    enum p_code_name id;
    union p_par par1;
    union p_par par2;
    int isfloat;
}p_code[500];//Ŀ�����洢��
int p_top;//Ŀ�������д������

int main_big;//��Ҫ��main��������Ŀռ��С
int main_entry;//�������
int main_tab;//main��tab��λ��

int is_error=0;
int is_over=0;

void getch(){//���ַ�����
    ch=fgetc(C0_code);
    //printf("%c\n",ch);
    //puts("1");
}
void error(int i){//������  ������������Ӧ�ı���
    printf("something is wrong,error code%d,in line%d\n",i,line);
    switch(i){
    case 1:
        puts("�ʷ�����,�˴�ӦΪ =");
        break;
    case 2:
        puts("�ʷ�����,0���治����ֱ�ӽ�����");
        break;
    case 3:
        puts("�ʷ�����,��ʶ������");
        break;
    case 4:
        puts("�쳣����");
        break;
    case 5:
        puts("�ʷ�����,ȱ���ҵ����Ż���˫����");
        break;
    case 6:
        puts("�﷨����,ȱ�����ͱ�ʶ��");
        break;
    case 7:
        puts("������󣬳��������ظ�");
        break;
    case 8:
        puts("�﷨���󣬴˴�ӦΪ��ʶ��");
        break;
    case 9:
        puts("�﷨���󣬴˴�ӦΪ�Ⱥ�");
        break;
    case 10:
        puts("������󣬳����������Ͳ�ƥ��");
        break;
    case 11:
        puts("������󣬳�Խ���ű�����");
        break;
    case 12:
        puts("�﷨�����ճ����ֺ�");
        break;
    case 13:
        puts("�﷨����ȱ���ҷ�����");
        break;
    case 14:
        puts("�﷨���󣬴˴�ӦΪ�޷�������");
        break;
    case 15:
        puts("�﷨���󣬲�������������������������");
        break;
    case 16:
        puts("�﷨���󣬴˴�ӦΪ���ͱ�ʶ��");
        break;
    case 17:
        puts("�﷨���󣬴˴�ӦΪ������");
        break;
    case 18:
        puts("�﷨���󣬴˴�ӦΪ������");
        break;
    case 19:
        puts("�﷨���󣬴˴�ӦΪ�һ�����");
        break;
    case 20:
        puts("�﷨���󣬱�ʶ����ֻ�ܽ�=��(");
        break;
    case 21:
        puts("�﷨���󣬴˴�ӦΪ������");
        break;
    case 22:
        puts("�﷨���󣬴˴�ӦΪ�Ⱥ�");
        break;
    case 23:
        puts("�﷨���󣬴˴�ӦΪ��ö�ٱ���");
        break;
    case 24:
        puts("�﷨���󣬴˴�ӦΪð��");
        break;
    case 25:
        puts("�﷨���󣬴˴�ӦΪvoid");
        break;
    case 26:
        puts("�﷨���󣬴˴�ӦΪmain");
        break;
    case 27:
        puts("������󣬱��������ظ�");
        break;
    case 28:
        puts("������󣬱�ʶ��δ�ڷ��ű����ҵ�");
        break;
    case 29:
        puts("��������������Խ��");
        break;
    case 30:
        puts("������󣬷Ǳ���������������");
        break;
    case 31:
        puts("������󣬺�����û�ҵ�");
        break;
    case 32:
        puts("������󣬲������Ͳ�ƥ��");
        break;
    case 33:
        puts("�������ֻ�������������ֵ");
        break;
    case 34:
        puts("�������switch����е��߼��ж�ֻ��Ϊ��ö�ٱ���������Ϊʵ��");
        break;
    case 35:
        puts("����������Ҹ�ֵ���Ͳ�ƥ��");
        break;
    case 36:
        puts("������󣬷���ֵ���Ͳ�ƥ��");
        break;
    case 37:
        puts("������󣬲������Ͳ�ƥ��");
        break;
    case 38:
        puts("�﷨������ֵ�����");
        break;
    case 39:
        puts("void�������Ͳ��ܷ���ֵ");
        break;
    }

    is_error=1;


}
void clear_token(int i){//�����ʱtoken����
    int ii;
    for(ii=0;ii<i;ii++){
        token[ii]=0;
    }
}
void getsym(){//�ʷ���������
    if(is_over)return;
    int ii=0;
    int i=0;//token��ʱָ��ÿ��һ���´� i=0;
    while(ch==' '||ch=='\t'||ch=='\n'){//���ַ�����
        if(ch=='\n')line++;
        getch();

    }
    if(ch=='*'){sym=times;getch();}
    else if(ch=='/'){sym=slash;getch();}
    else if(ch==';'){sym=semicolon;getch();}
    else if(ch==':'){sym=colon;getch();}
    else if(ch=='+'){sym=add;getch();}
    else if(ch=='-'){sym=minus;getch();}
    else if(ch==','){sym=comma;getch();}
    else if(ch=='('){sym=leftparent;getch();}
    else if(ch==')'){sym=rightparent;getch();}
    else if(ch=='['){sym=leftbracket;getch();}
    else if(ch==']'){sym=rightbracket;getch();}
    else if(ch=='{'){sym=leftcurlybracket;getch();}
    else if(ch=='}'){sym=rightcurlybracket;getch();}
    else if(ch=='<'){
        getch();
        if(ch=='='){
            sym=lessorequal;
            getch();
        }
        else{
            sym=less;
        }
    }
    else if(ch=='>'){
        getch();
        if(ch=='='){
            sym=moreorequal;
            getch();
        }
        else{
            sym=more;
        }
    }
    else if(ch=='='){
        getch();
        if(ch=='='){
            sym=equal;
            getch();
        }
        else{
            sym=becames;
        }
    }
    else if(ch=='!'){
        getch();
        if(ch=='='){
            sym=notequal;
            getch();
        }
        else{
            error(1);
        }
    }
    else if(ch=='\''){
        getch();
        zf=ch;
        getch();
        if(ch!='\''){
            error(5);
        }
        else{
            getch();
        }
        sym=character;

    }
    else if(ch=='\"'){
        getch();
        while(ch==32||ch==33||(ch>=35&&ch<=126)){
            token[i]=ch;
            i++;
            getch();
        }
        if(ch!='\"'){
            error(5);
        }
        else{
            getch();
        }
        strcpy(zfc,token);
        clear_token(i);
        i=0;
        sym=string;
    }
    else if(ch>='1'&&ch<='9'){
        token[i]=ch;
        i++;
        getch();
        while(ch>='0'&&ch<='9'){
            token[i]=ch;
            i++;
            getch();
        }
        if(ch=='.'){
            token[i]=ch;
            i++;
            getch();
            while(ch>='0'&&ch<='9'){
                token[i]=ch;
                i++;
                getch();
            }
            realnum=atof(token);
            clear_token(i);
            i=0;
            sym=realnumber;
        }
        else{
            integernum=atoi(token);
            clear_token(i);
            i=0;
            sym=unsignedinteger;
        }
    }
    else if(ch=='0'){
        getch();
        if(ch>='0'&&ch<='9'){
            error(2);
        }
        else{
            if(ch=='.'){
                token[0]='0';
                token[1]='.';
                i=2;
                getch();
                while(ch>='0'&&ch<='9'){
                    token[i]=ch;
                    i++;
                    getch();
                }
                realnum=atof(token);
                clear_token(i);
                i=0;
                sym=realnumber;

            }
            else{
                sym=integer;
                integernum=0;
            }
        }
    }
    else if((ch>='A'&&ch<='Z')||ch=='_'||(ch>='a'&&ch<='z')){
        token[i]=ch;
        i++;
        getch();
        while((ch>='A'&&ch<='Z')||ch=='_'||(ch>='a'&&ch<='z')||(ch>='0'&&ch<='9')){
            if(i>20){
                error(3);
            }
            token[i]=ch;
            i++;
            getch();
        }
        token[i]=0;
        if(strcmp(token,"const")==0){sym=constsym;}
        else if(strcmp(token,"int")==0){sym=intsym;}
        else if(strcmp(token,"float")==0){sym=floatsym;}
        else if(strcmp(token,"char")==0){sym=charsym;}
        else if(strcmp(token,"void")==0){sym=voidsym;}
        else if(strcmp(token,"main")==0){sym=mainsym;}
        else if(strcmp(token,"if")==0){sym=ifsym;}
        else if(strcmp(token,"else")==0){sym=elsesym;}
        else if(strcmp(token,"while")==0){sym=whilesym;}
        else if(strcmp(token,"switch")==0){sym=switchsym;}
        else if(strcmp(token,"case")==0){sym=casesym;}
        else if(strcmp(token,"scanf")==0){sym=scanfsym;}
        else if(strcmp(token,"printf")==0){sym=printfsym;}
        else if(strcmp(token,"return")==0){sym=returnsym;}
        else{
            sym=ident;
            ii=0;
            for(ii=0;ii<i;ii++){
                if(token[ii]>='A'&&token[ii]<='Z')
                token[ii]+=32;
            }
            strcpy(id,token);
        }
        clear_token(i);
        i=0;
    }
    else if(ch==EOF){
        puts("this file is end");
        is_over=1;
        return;
        //exit(1);
    }
    else{

        error(4);
        getch();
    }
    //printf("%d:~~~%s\n",line,symbol_rem[sym]);


}
void enter(int add,char *name,int lev,int type,int kind,int degree,int is_array){//������ű���
    if(add>500){error(11);printf("����̫���ˣ������ű������ˣ��������");exit(1);}
    strcpy(tab[add].name,name);
    tab[add].lev=lev;
    tab[add].type=type;
    tab[add].kind=kind;
    tab[add].degree=degree;
    tab[add].is_array=is_array;
    if(kind==2){
        if(add==1){tab[add].offset=3;}
        else{
            tab[add].offset=tab[add-1].offset;
        }
        switch(type){
        case 0:tab[add].val.intval=integernum;break;
        case 1:tab[add].val.floatval=realnum;break;
        case 2:tab[add].val.charval=zf;break;
        default:tab[add].val.intval=0;
    }
    }
    if(kind==1){
        if(add==1){tab[add].offset=3;}
        else{
            tab[add].offset=tab[add-1].offset+tab[add-1].degree;
        }

    }
    if(kind==0){
        tab[add].offset=3;
        tab[add].val.funcval.num=0;
    }

}
void gen(enum p_code_name name,int par1,int par2){
    p_code[p_top].id=name;
    p_code[p_top].par1.zheng=par1;
    p_code[p_top].par2.zheng=par2;
    p_code[p_top].isfloat=0;
    //printf("%s,%d,%d!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",P_code_name_name[p_code[p_top].id],p_code[p_top].par1.zheng,p_code[p_top].par2.zheng);
    p_top++;
}
void genf(enum p_code_name name,int par1,float par2){
    p_code[p_top].id=name;
    p_code[p_top].par1.zheng=par1;
    p_code[p_top].par2.shi=par2;
    p_code[p_top].isfloat=1;
    p_top++;
}
void enter_function_value(int type){
    int adr=tabtop;
    for(adr=tabtop;adr>0;adr--){
        if(tab[adr].kind==0)
            break;
    }
    tab[adr].val.funcval.num++;
    tab[adr].val.funcval.parameter[tab[adr].val.funcval.num-1]=type;
    tab[adr].val.funcval.funcpoint=p_top;
}
void enter_function_big(int big){
    int adr=tabtop;
    for(adr=tabtop;adr>0;adr--){
        if(tab[adr].kind==0)
            break;
    }
    tab[adr].val.funcval.big=big;
    tab[adr].val.funcval.funcpoint=p_top;

}
int find(char* name){//����name�Ƿ�����ű���ĳ���������ҵ�������λ�ã�û�з���0
    int i=tabtop;
    for(i=tabtop;i>0;i--){
        if(tab[i].kind==0){
            return 0;//��ǰ�ҵ����� ֱ�ӽ���
        }
        if(strcmp(name,tab[i].name)==0){
            return i;
        }
    }
    return 0;
}
int lookup(char *name){//����ű�����Ѱ������Ϊname�ķ����ڷ��ű��е�λ�á�
    int i=tabtop;
    for(i=tabtop;i>0;i--){//�Ҿֲ�����
        if(tab[i].kind==0){break;}
        if(strcmp(name,tab[i].name)==0){
            return i;
        }
    }
    for(i=1;i<=tabtop;i++){
        if(tab[i].lev==1)continue;
        else{
            if(strcmp(name,tab[i].name)==0){
            return i;
            }
        }
    }
    return 0;
}
int lookupmain(char *name){//����ű� ֻ��ȫ�ֱ���
    int i=0;

    for(i=1;i<=tabtop;i++){
        if(tab[i].lev==1)continue;
        else{
            if(strcmp(name,tab[i].name)==0){
            return i;
            }
        }
    }
    return 0;
}
int factor(){//�����ӣ�    ::= ����ʶ����������ʶ������[�������ʽ����]������������|��ʵ����|���ַ��������з���ֵ����������䣾|��(�������ʽ����)��
    int i=0;
    int paradr=4;
    int tp=0;//���ͱ�ʶ��0���ַ���1���Σ�2ʵ��
    int ftp=0;
    if(sym==ident){
        i=lookup(id);
        if(i==0){
            error(28);
            gen(LIT,0,0);
            getsym();
            return 1;
        }
        getsym();
        if(tab[i].type==0){tp=1;}
        if(tab[i].type==1){tp=2;}
        if(sym==leftbracket){
            if(tab[i].kind!=1){
                error(30);
                gen(LIT,0,0);
                getsym();
                return 1;
            }
            getsym();

            expression();
            if(sym==rightbracket){
                getsym();
            }
            else{
                error(13);
            }
        }
        else if(sym==leftparent){
            if(tab[i].kind!=0){//�ж��ǲ��Ǻ���
                i=lookupmain(tab[i].name);
                if(i==0||tab[i].kind!=0){
                    error(31);
                    gen(LIT,0,0);
                    getsym();
                    return 1;
                }
            }

            getsym();
            if(sym==rightparent)//���޲���
            {   getsym();
                if(tab[i].val.funcval.num!=0){error(32);}
                gen(INT,i,tab[i].val.funcval.big);//����洢ջ
                gen(STI,p_top+2,2);
                gen(JMP,0,tab[i].val.funcval.funcpoint);

            }
            else{
                ftp=expression();
                if(ftp==2&&tab[i].val.funcval.parameter[0]==0||ftp==2&&tab[i].val.funcval.parameter[0]==2){
                    error(37);
                }
                paradr++;
                while(sym==comma){
                    getsym();
                    ftp=expression();
                    if(ftp==2&&tab[i].val.funcval.parameter[paradr-4]==0||ftp==2&&tab[i].val.funcval.parameter[paradr-4]==2){
                    error(37);
                    }
                    paradr++;
                    if(paradr-4>tab[i].val.funcval.num){error(32);}
                }
                if(sym==rightparent)getsym();
                else{
                    error(17);
                }
                gen(INT,i,tab[i].val.funcval.big);//����洢ջ
                for(paradr-=1;paradr>=4;paradr--){
                    gen(STO,1,paradr);
                }

                gen(STI,p_top+2,2);
                gen(JMP,0,tab[i].val.funcval.funcpoint);

            }
            gen(LOD,1,3);
            gen(QUT,0,0);
        }
        if(tab[i].kind==1&&tab[i].is_array==0){
            gen(LOD,tab[i].lev,tab[i].offset);
        }else if(tab[i].kind==1&&tab[i].is_array==1){
            gen(LIT,0,tab[i].offset);
            gen(ADD,0,0);
            gen(LODT,tab[i].lev,0);
        }
        else if(tab[i].kind==2){
            switch(tab[i].type){
                case 0:gen(LIT,0,tab[i].val.intval);break;
                case 1:genf(LIT,0,tab[i].val.floatval);break;
                case 2:gen(LIT,0,tab[i].val.charval);break;
            }
        }
    }
    else if(integerjudge()){gen(LIT,0,integernum);getsym();tp=1;}
    else if(realnumjudge()){
            genf(LIT,0,realnum);
            getsym();
            tp=2;
            }
    else if(sym==character){gen(LIT,0,zf);getsym();tp=0;}
    else if(sym==leftparent){
        getsym();
        tp=expression();
        if(sym==rightparent)getsym();
        else{error(17);}
    }
    else{
        error(38);
    }
    return tp;
}
int terms(){//���     ::= �����ӣ�{���˷�������������ӣ�}
    int tp=0;
    int tpf=0;
    int mul_judge=0;
    tpf=factor();
    if(tp<tpf){
        tp=tpf;
    }
    while(sym==times||sym==slash){
        if(sym==times)mul_judge=1;
        else{mul_judge=0;}
        getsym();
        tpf=factor();
        if(tp<tpf){
        tp=tpf;
        }
        if(mul_judge)gen(MUL,0,0);
        else{gen(DIV,0,0);}
    }
    return tp;
}
int expression(){//�����ʽ��    ::= �ۣ������ݣ��{���ӷ�����������}
    int add_judge=1;
    int tp=0;
    int tpt=0;
    int genadd=0;
    if(sym==add||sym==minus){
        gen(LIT,0,0);
        if(sym==add){
            genadd=1;

        }
        else{
            genadd=2;

        }
        getsym();
    }
    tpt=terms();
    if(genadd!=0){
        if(genadd==1){
            gen(ADD,0,0);
        }
        else{
            gen(SUB,0,0);
        }
    }
    if(tp<tpt)tp=tpt;
    while(sym==add||sym==minus){
        if(sym==add)add_judge=1;
        else{add_judge=0;}
        getsym();
        tpt=terms();
        if(tp<tpt)tp=tpt;
        if(add_judge)gen(ADD,0,0);
        else{gen(SUB,0,0);}
    }
    //puts("This is a expression");
    return tpt;
}
void ifstatement(){//��������䣾  ::=  if ��(������������)������䣾��else����䣾��
    int logic_sy;
    int jmpoint1;
    int jmpoint2;
    int tp=0;
    getsym();
    if(sym!=leftparent){error(21);}
    getsym();
    expression();

    if(sym==more||sym==moreorequal||sym==less||sym==lessorequal||sym==notequal||sym==equal){
        if(sym==more)logic_sy=1;
        else if(sym==moreorequal)logic_sy=2;
        else if(sym==less)logic_sy=3;
        else if(sym==lessorequal)logic_sy=4;
        else if(sym==notequal)logic_sy=5;
        else if(sym==equal)logic_sy=6;
        getsym();
        expression();

        gen(LOG,0,logic_sy);
    }
    if(sym!=rightparent){error(17);}
    gen(JPC,0,0);
    jmpoint1=p_top-1;
    getsym();
    statement();
    p_code[jmpoint1].par2.zheng=p_top;
    if(sym==elsesym){
        gen(JMP,0,0);
        p_code[jmpoint1].par2.zheng=p_top;
        jmpoint2=p_top-1;
        getsym();
        statement();
        p_code[jmpoint2].par2.zheng=p_top;
    }
    //puts("This is a if statement");
}
void whilestatement(){//ѭ�����
    int logic_sy;
    int jmpoint1;
    int jmpoint2;
    int tp=0;
    jmpoint1=p_top;
    getsym();
    if(sym!=leftparent){error(21);}
    getsym();
    expression();

    if(sym==more||sym==moreorequal||sym==less||sym==lessorequal||sym==notequal||sym==equal){
        if(sym==more)logic_sy=1;
        else if(sym==moreorequal)logic_sy=2;
        else if(sym==less)logic_sy=3;
        else if(sym==lessorequal)logic_sy=4;
        else if(sym==notequal)logic_sy=5;
        else if(sym==equal)logic_sy=6;
        getsym();
        expression();

        gen(LOG,0,logic_sy);
    }
    if(sym!=rightparent){error(17);}
    getsym();
    gen(JPC,0,0);
    jmpoint2=p_top-1;
    statement();
    gen(JMP,0,jmpoint1);
    p_code[jmpoint2].par2.zheng=p_top;
    //puts("This is a while statement");
}
void callorassign(){//���������߸�ֵ���
    int paradr=4;
    int i=0;
    int tpl;
    int tpr;
    int ftp=0;
    i=lookup(id);
    if(i==0){
        error(28);
        while(sym!=semicolon){
            getsym();
        }
        return;
    }
    tpl=tab[i].type;
    getsym();
    if(sym==leftparent){//call
        if(tab[i].kind!=0){
            i=lookupmain(tab[i].name);
            if(i==0||tab[i].kind!=0){
                error(31);
                while(sym!=semicolon){
                    getsym();
                }
                return;
            }
        }

        getsym();
        if(sym==rightparent){
                if(tab[i].val.funcval.num!=0){error(32);}
                gen(INT,i,tab[i].val.funcval.big);
                gen(STI,p_top+2,2);
                gen(JMP,0,tab[i].val.funcval.funcpoint);
                gen(QUT,0,0);
                getsym();
                //puts("this is a call statement");return;

        }
        else{
            ftp=expression();
            if(ftp==2&&tab[i].val.funcval.parameter[paradr-4]==0||ftp==2&&tab[i].val.funcval.parameter[paradr-4]==2){
                error(37);
            }
            //gen(STO,1,paradr);
            paradr++;
            if(paradr-4>tab[i].val.funcval.num){error(32);}
            while(sym==comma){
                getsym();
                ftp=expression();
                if(ftp==2&&tab[i].val.funcval.parameter[paradr-4]==0||ftp==2&&tab[i].val.funcval.parameter[paradr-4]==2){
                    error(37);
                }
                //gen(STO,1,paradr);
                paradr++;
                if(paradr-4>tab[i].val.funcval.num){error(32);}
            }
            if(sym==rightparent){
                getsym();
            }
            else{
                error(17);
            }
            gen(INT,i,tab[i].val.funcval.big);
            for(paradr-=1;paradr>=4;paradr--){
                    gen(STO,1,paradr);
                }
            gen(STI,p_top+2,2);
            gen(JMP,0,tab[i].val.funcval.funcpoint);
            gen(QUT,0,0);
        }
        //puts("this is a call statement");
        return;
    }
    if(sym==leftbracket){
        getsym();

        expression();
        if(sym==rightbracket){
            getsym();
        }
        else{error(13);}
    }
    if(tab[i].is_array){
        gen(LOA,tab[i].lev,tab[i].offset);
        gen(ADD,0,0);
    }
    else{
        gen(LOA,tab[i].lev,tab[i].offset);
    }
    if(sym==becames){
        getsym();
    }
    else{
        error(22);
        while(sym!=semicolon){
            getsym();
        }
        return;
    }
    tpr=expression();
    if(tpl==0&&tpr==2||tpr==2&&tpl==2)error(35);
    if(tpl==1&&tpr==0||tpl==1&&tpr==1){
        gen(STT,0,1);
    }
    else{
        gen(STT,0,0);
    }

    //puts("this is a assign statement");
}
void readstatement(){//�����
    getsym();
    int i=0;
    if(sym!=leftparent){
            error(21);
            while(sym!=semicolon){
                getsym();
            }
            return;
    }

    else{
        getsym();
    }
    if(sym==ident){

        i=lookup(id);
        if(i==0){
        error(28);
        while(sym!=semicolon){
            getsym();
        }
        return;
        }
        if(tab[i].kind!=1){
            error(33);
            while(sym!=semicolon){
                getsym();
            }
            return;
        }

        if(tab[i].type==0){
            gen(rei,0,0);
        }
        else if(tab[i].type==1){
            gen(ref,0,0);
        }
        else if(tab[i].type==2){
            gen(rec,0,0);
        }
        gen(STO,tab[i].lev,tab[i].offset);
        getsym();
    }
    else{
        error(8);
    }
    while(sym==comma){
        getsym();
        if(sym==ident){
            i=lookup(id);
            if(i==0){
            error(28);
            while(sym!=semicolon){
                getsym();
            }
            return;
            }
            if(tab[i].kind!=1){
                error(33);
                while(sym!=semicolon){
                    getsym();
                }
                return;
            }
            if(tab[i].type==0){
                gen(rei,0,0);
            }
            else if(tab[i].type==1){
                gen(ref,0,0);
            }
            else if(tab[i].type==2){
                gen(rec,0,0);
            }
            gen(STO,tab[i].lev,tab[i].offset);
            getsym();
        }
        else{
            error(8);
        }
    }
    if(sym==rightparent)getsym();
    else{error(17);}
    //puts("this is a read statement");
}
void writestatement(){//д���
    int tp;
    getsym();
    if(sym==leftparent){getsym();}
    else{error(21);}
    if(sym==string){
        strcpy(hc[hc_top],zfc);
        gen(ws,0,hc_top);
        hc_top++;

        getsym();
        if(sym==comma){
            getsym();
            tp=expression();
            gen(wr,0,tp);
        }
    }
    else{
        tp=expression();
        gen(wr,0,tp);
    }
    if(sym==rightparent)getsym();
    else{error(17);}
    //puts("this is a write statement");

}
void switchstatement(){//switch���
    int jmpoint=0;
    int jmpoint2[999];
    int jpd=0;
    int tp=0;
    int i;
    getsym();
    if(sym==leftparent)getsym();
    else{error(21);}
    tp=expression();
    if(tp>1)error(34);
    if(sym==rightparent)getsym();
    else{error(17);}

    if(sym==leftcurlybracket)getsym();
    else{error(18);}
    while(sym==casesym){
        getsym();
        if(integerjudge()){
            gen(LIT,0,integernum);
            gen(LOGT,0,6);
            gen(JPC,0,0);
            jmpoint=p_top;
            getsym();
        }
        else if(sym==character){
            gen(LIT,0,zf);
            gen(LOGT,0,6);
            gen(JPC,0,0);
            jmpoint=p_top;
            getsym();
        }
        else{error(23);}
        if(sym==colon)getsym();
        else{error(24);}
        statement();
        gen(JMP,0,0);
        jmpoint2[jpd]=p_top-1;
        jpd++;
        p_code[jmpoint-1].par2.zheng=p_top;
    }
    if(sym==rightcurlybracket)getsym();
    else{error(19);}
    for(i=0;i<jpd;i++){
        p_code[jmpoint2[i]].par2.zheng=p_top;
    }

    //puts("this is a switch statement");
}
void returnstatement(){//�������
    getsym();
    int i;
    int isvoid=0;
    i=tabtop;
    for(i=tabtop;i>0;i--){
        if(tab[i].kind==0){
            if(tab[i].type==3){
                isvoid=1;
            }
            break;
        }
    }
    if(sym==leftparent){
            if(isvoid){
                error(39);
            }
        getsym();
        expression();
        gen(STO,1,3);
        if(sym==rightparent)getsym();
        else{error(17);}
        gen(JMB,0,0);


    }
    else{
       gen(JMB,0,0);
    }
    //puts("this is a return statement");
}

void statement(){//���



        switch(sym){//���ֿ��ܵ���� Ҫ��������������һ��sym
            case ifsym:ifstatement();break;
            case whilesym:whilestatement();break;
            case leftcurlybracket:{
                getsym();
                while(sym!=rightcurlybracket){

                    statement();

                }
                getsym();
                break;
            }
            case ident:{
                callorassign();
                if(sym==semicolon){getsym();}
                else{error(12);}
                break;
            }
            case scanfsym:
                readstatement();
                if(sym==semicolon){getsym();}
                else{error(12);}
                break;
            case printfsym:
                writestatement();
                if(sym==semicolon){getsym();}
                else{error(12);}
                break;
            case semicolon:getsym();break;
            case switchsym:switchstatement();break;
            case returnsym:
                returnstatement();
                if(sym==semicolon){getsym();}
                else{error(12);}
                break;


    }
}
int vardefine(){//�����˾��ı���˵���ݹ��½��ӳ���
    int v_type=-1;//��ʱ ��ʶ��������,��ʼΪδʶ������
    int v_degree=1;//��ʱ����ʼ����
    int v_add;//��Ҫ�����ķ��ű���
    int v_array=0;
    int v_big=0;
    while(sym==intsym||sym==floatsym||sym==charsym){
        switch(sym){//��¼��������

        case intsym:
            v_type=0;
            break;
        case floatsym:
            v_type=1;
            break;
        case charsym:
            v_type=2;
            break;
        default:
            v_type=-1;
            error(6);
        }
        getsym();
        if(sym!=ident){
            error(8);
        }
        else{
            if(v_add=find(id)){
                error(27);//�����ظ�����

            }
            if(v_add==0){
                tabtop++;
                v_add=tabtop;
            }
        }
        getsym();
        if(sym==leftbracket){//������ʶ��
            v_array=1;
            getsym();
            if(sym==unsignedinteger){
                v_degree=integernum;
                getsym();
                if(sym==rightbracket){
                    getsym();
                }
                else{
                    error(13);
                }
            }
            else{
                error(14);
            }
        }
        enter(v_add,id,1,v_type,1,v_degree,v_array);

        v_degree=1;
        v_array=0;
        while(sym==comma){
            getsym();
            if(sym!=ident){
                error(8);
            }
            else{
                if(v_add=find(id)){
                    error(27);//�����ظ�����

                }
                if(v_add==0){
                    tabtop++;
                    v_add=tabtop;
                }
            }
            getsym();
            if(sym==leftbracket){//������ʶ��
                v_array=1;
                getsym();
                if(sym==unsignedinteger){
                    v_degree=integernum;
                    getsym();
                    if(sym==rightbracket){
                        getsym();
                    }
                    else{
                        error(13);
                    }
                }
                else{
                    error(14);
                }
            }
            enter(v_add,id,1,v_type,1,v_degree,v_array);

            v_degree=1;
            v_array=0;
        }
        if(sym!=semicolon){error(12);}
        else{
            getsym();
        }
    //puts("this is a var define in a function");
    }
    v_big=tab[tabtop].offset+v_degree-1;
    return v_big;
}
void vardefine_inmain(){//����˵���ݹ��½��ӳ������������е��ã�ͬʱ������������
    int var_over=0;//��Ǳ���������Ϊ1ʱ���������������Ѿ���ɣ����뺯������׶�
    int v_type=-1;
    int v_degree=1;
    int v_add;
    int v_array=0;
    int v_big=0;
    int f_big=0;
    while(sym==intsym||sym==floatsym||sym==charsym||sym==voidsym){
        if(sym==voidsym){var_over=1;}//һ������void��ô���������Ѿ�������
        switch(sym){
        case intsym:
            v_type=0;
            break;
        case floatsym:
            v_type=1;
            break;
        case charsym:
            v_type=2;
            break;
        case voidsym:
            v_type=3;
            break;
        default:
            v_type=-1;
        }
        getsym();
        if(sym==mainsym){
            gen(INT,-1,v_big);
            return;
        }
        if(sym!=ident){
            error(8);
        }
        else{
            if(v_add=find(id)){

                error(27);
            }
            else{
                tabtop++;
                v_add=tabtop;
            }
        }
        getsym();
        if(sym==leftparent){//���������� ���뺯���׶�
            enter(v_add,id,0,v_type,0,1,0);
            var_over=1;
            getsym();
            if(sym==intsym||sym==floatsym||sym==charsym){
                switch(sym){
                    case intsym:
                        v_type=0;
                        break;
                    case floatsym:
                        v_type=1;
                        break;
                    case charsym:
                        v_type=2;
                        break;
                    default:
                        v_type=-1;
                    }
                getsym();
                if(sym!=ident){
                    error(8);
                }
                else{
                   if(v_add=find(id)){

                        error(27);//�����ظ�����
                    }
                    if(v_add==0){
                        tabtop++;
                        v_add=tabtop;
                    }
                }

                enter(v_add,id,1,v_type,1,1,0);
                enter_function_value(v_type);
                getsym();
                while(sym==comma){
                    getsym();
                    if(sym==intsym||sym==floatsym||sym==charsym){
                        switch(sym){
                        case intsym:
                            v_type=0;
                            break;
                        case floatsym:
                            v_type=1;
                            break;
                        case charsym:
                            v_type=2;
                            break;
                        default:
                            v_type=-1;
                        }
                        getsym();
                        if(sym!=ident){
                            error(8);
                        }
                        else{
                            if(v_add=find(id)){
                                error(27);
                            }
                            else{
                                tabtop++;
                                v_add=tabtop;
                            }
                        }

                        enter(v_add,id,1,v_type,1,1,0);
                        enter_function_value(v_type);
                        getsym();
                    }
                    else{
                        error(16);
                    }
                }
            }
            if(sym!=rightparent){
                error(17);
            }
            getsym();
            if(sym!=leftcurlybracket){
                error(18);
            }
            getsym();
            constdefine(1);
            f_big=vardefine();
            enter_function_big(f_big);

            while(sym!=rightcurlybracket){

                    statement();

            }
            gen(JMB,0,0);


            getsym();
            //puts("this is a function define in main");
            continue;
        }
        if(sym==leftbracket&&var_over==0){//������ʶ��
            v_array=1;
            getsym();
            if(sym==unsignedinteger){
                v_degree=integernum;
                getsym();
                if(sym==rightbracket){
                    getsym();
                }
                else{
                    error(13);
                }
            }
            else{
                error(14);
            }
        }
        else if(var_over==1&&sym==leftbracket){//��������Ϊ����
            error(15);
        }
        enter(v_add,id,0,v_type,1,v_degree,v_array);
        v_big=tab[tabtop].offset+v_degree;
        v_degree=1;
        v_array=0;
        while(sym==comma&&var_over==0){
            getsym();
            if(sym!=ident){
                error(8);
            }
            else{
            if(v_add=find(id)){
                error(7);
            }
            else{
                tabtop++;
                v_add=tabtop;
            }
            }
            getsym();
            if(sym==leftbracket){//������ʶ��
                v_array=1;
                getsym();
                if(sym==unsignedinteger){
                    v_degree=integernum;
                    getsym();
                    if(sym==rightbracket){
                        getsym();
                    }
                    else{
                        error(13);
                    }
                }
                else{
                    error(14);
                }
            }
            enter(v_add,id,0,v_type,1,v_degree,v_array);
            v_big=tab[tabtop].offset+v_degree;
            v_degree=1;
            v_array=0;
        }
        if(sym==semicolon&&var_over==0){getsym();}//puts("this is a var define in main");
        else{
            if(sym!=semicolon&&var_over==0){error(12);}
        }
        //enter(v_add,id,0,v_type,1,v_degree);�﷨���� �ݲ�����ű�
    }
    gen(INT,-1,v_big);
}
int integerjudge(){//�ǲ������ε��ж�
    int minus_judge=0;
    if(sym==integer)return 1;
    else if(sym==add||sym==minus){
        if(sym==minus)minus_judge=1;
        getsym();
        if(sym==unsignedinteger){if(minus_judge)integernum=-integernum;return 1;}
    }
    else if(sym==unsignedinteger)return 1;
    else{return 0;}
}
int realnumjudge(){//ʵ���ж�
    int minus_judge=0;
    if(sym==realnumber)return 1;
    else if(sym==add||sym==minus){
        if(sym==minus)minus_judge=1;
        getsym();
        if(sym==realnumber){if(minus_judge)realnum=-realnum;return 1;}
    }
    else return 0;
}
void constdefine(int c_lev){//����˵���ݹ��½��ӳ���
    int c_type=-1;//��ʱ ��ʶ��������,��ʼΪδʶ������
    int c_add;//��Ҫ�����ķ��ű���
    while(sym==constsym){
        getsym();
        switch(sym){//��¼��������
        case intsym:
            c_type=0;
            break;
        case floatsym:
            c_type=1;
            break;
        case charsym:
            c_type=2;
            break;
        default:
            c_type=-1;
            error(6);
        }
        getsym();
        if(sym==ident){
            if(c_add=find(id)){
                error(7);//�����ظ�����
            }
            if(c_add==0){
                tabtop++;
                c_add=tabtop;
            }
        }
        else{error(8);}
        getsym();
        if(sym!=becames){
            error(9);
        }
        getsym();
        if(integerjudge()&&(c_type==0||c_type==2)){

            getsym();
        }
        else if(realnumjudge()&&c_type==1){

            getsym();
        }
        else if(sym==character&&(c_type==0||c_type==2)){

            getsym();
        }
        else{
            error(10);//ǰ������һ�����ж�
        }
        enter(c_add,id,c_lev,c_type,2,1,0);


        while(sym==comma){
            getsym();
            if(sym==ident){
                if(c_add=find(id)){
                    error(7);//�����ظ�����
                }
                if(c_add==0){
                    tabtop++;
                    c_add=tabtop;
                }
            }
            else{error(8);}
            getsym();
            if(sym!=becames){
                error(9);
            }
            getsym();
            if(integerjudge()&&c_type==0){

            getsym();
            }
            else if(realnumjudge()&&c_type==1){

                getsym();
            }
            else if(sym==character&&c_type==2){

                getsym();
            }
            else{
                error(10);//ǰ������һ�����ж�
            }
            enter(c_add,id,c_lev,c_type,2,1,0);


        }
        if(sym!=semicolon){
            error(12);
        }
        //puts("this is a constdefine in main");
        getsym();
    }
}
void mainfun(){//������

    if(sym==mainsym)getsym();
    else{error(26);}
    if(sym==leftparent)getsym();
    else{error(21);}
    if(sym==rightparent)getsym();
    else{error(17);}
    if(sym==leftcurlybracket)getsym();
    else{error(18);}
    tabtop++;
    enter(tabtop,"MAIN",0,0,0,1,0);
    main_tab=tabtop;
    constdefine(1);
    main_big=vardefine();
    main_entry=p_top;

    while(sym!=rightcurlybracket){

            statement();

    }
    getsym();

    //puts("this's main");
}
void program(){//�ݹ�����ӳ���֮����
    int mainpoint1;
    int mainpoint2;
    int mainpoint3;
    int startpoint;
    getch();
    getsym();//��ȡ��һ����
    mainpoint2=p_top;
    gen(JMP,0,0);
    constdefine(0);
    vardefine_inmain();
    startpoint=p_top-1;
    mainpoint1=p_top;
    gen(INT,1,0);
    gen(STI,999999,2);
    mainpoint3=p_top;

    gen(JMP,0,0);

    mainfun();
    p_code[mainpoint1].par2.zheng=main_big;
    p_code[mainpoint1].par1.zheng=main_tab;
    p_code[mainpoint2].par2.zheng=startpoint;
    p_code[mainpoint3].par2.zheng=main_entry;

}
void interupt(){//����ִ�к���
    //�洢����
    //������ p_code
    enum p_code_name p;//ָ��Ĵ���
    int pc=0;//ָ���ַ�Ĵ���
    //�洢ջ

    int mbase=0;//����ַ�Ĵ���
    int mtop=0;//�洢ջջ���Ĵ���
    //����ջ
    struct arun{
        int isfloat;
        int zheng;
        double shi;
    }run[500],mem[500];
    int rtop=0;//����ջջ���Ĵ���

    while(pc<p_top){//�ڳ���û�н����������
        //printf("%d\n",pc);
        p=p_code[pc].id;//��ȡ������
        int l;
        int A;
        switch(p){
        case ADD:
            if((!run[rtop-1].isfloat)&&(!run[rtop].isfloat)){run[rtop-1].zheng=run[rtop-1].zheng+run[rtop].zheng;}
            else if((run[rtop-1].isfloat)&&(!run[rtop].isfloat)){run[rtop-1].shi=run[rtop-1].shi+(double)run[rtop].zheng;}
            else if((!run[rtop-1].isfloat)&&(run[rtop].isfloat)){run[rtop-1].shi=(double)run[rtop-1].zheng+run[rtop].shi;run[rtop-1].isfloat=1;}
            else if((run[rtop-1].isfloat)&&(run[rtop].isfloat)){run[rtop-1].shi=run[rtop-1].shi+run[rtop].shi;}
            rtop--;
            pc++;
            break;

        case SUB:
            if((!run[rtop-1].isfloat)&&(!run[rtop].isfloat)){run[rtop-1].zheng=run[rtop-1].zheng-run[rtop].zheng;}
            else if((run[rtop-1].isfloat)&&(!run[rtop].isfloat)){run[rtop-1].shi=run[rtop-1].shi-(double)run[rtop].zheng;}
            else if((!run[rtop-1].isfloat)&&(run[rtop].isfloat)){run[rtop-1].shi=(double)run[rtop-1].zheng-run[rtop].shi;run[rtop-1].isfloat=1;}
            else if((run[rtop-1].isfloat)&&(run[rtop].isfloat)){run[rtop-1].shi=run[rtop-1].shi-run[rtop].shi;}
            rtop--;
            pc++;
            break;

        case MUL:
            if((!run[rtop-1].isfloat)&&(!run[rtop].isfloat)){run[rtop-1].zheng=run[rtop-1].zheng*run[rtop].zheng;}
            else if((run[rtop-1].isfloat)&&(!run[rtop].isfloat)){run[rtop-1].shi=run[rtop-1].shi*(double)run[rtop].zheng;}
            else if((!run[rtop-1].isfloat)&&(run[rtop].isfloat)){run[rtop-1].shi=(double)run[rtop-1].zheng*run[rtop].shi;run[rtop-1].isfloat=1;}
            else if((run[rtop-1].isfloat)&&(run[rtop].isfloat)){run[rtop-1].shi=run[rtop-1].shi*run[rtop].shi;}
            rtop--;
            pc++;
            break;

        case DIV:
            if((!run[rtop-1].isfloat)&&(!run[rtop].isfloat)){
                    run[rtop-1].zheng=run[rtop-1].zheng/run[rtop].zheng;
            }
            else if((run[rtop-1].isfloat)&&(!run[rtop].isfloat)){
                run[rtop-1].shi=run[rtop-1].shi/(double)run[rtop].zheng;
                }
            else if((!run[rtop-1].isfloat)&&(run[rtop].isfloat)){
                run[rtop-1].shi=(double)run[rtop-1].zheng/run[rtop].shi;run[rtop-1].isfloat=1;
                }
            else if((run[rtop-1].isfloat)&&(run[rtop].isfloat)){
                run[rtop-1].shi=run[rtop-1].shi/run[rtop].shi;
                }
            rtop--;
            pc++;
            break;

        // LIT 0 A,��������A����ջ��
        case LIT:
            rtop++;
            if(p_code[pc].isfloat){
                run[rtop].isfloat=1;
                run[rtop].shi=p_code[pc].par2.shi;
            }
            else if(!p_code[pc].isfloat){
                run[rtop].isfloat=0;
                run[rtop].zheng=p_code[pc].par2.zheng;
            }
            pc++;
            break;
        // LOD l A,�����Ϊl����Ե�ַΪA�ı�������ջ��
        case LOD:
            rtop++;
            l=p_code[pc].par1.zheng;
            A=p_code[pc].par2.zheng;
            if(l==0){
                if(mem[A].isfloat){
                    run[rtop].isfloat=1;
                    run[rtop].shi=mem[A].shi;
                }
                else{
                    run[rtop].isfloat=0;
                    run[rtop].zheng=mem[A].zheng;
                }
            }
            else{
                if(mem[A+mbase].isfloat){
                    run[rtop].isfloat=1;
                    run[rtop].shi=mem[A+mbase].shi;
                }
                else{
                    run[rtop].isfloat=0;
                    run[rtop].zheng=mem[A+mbase].zheng;
                }
            }
            pc++;
            break;
        //LODT l 0,����ַΪջ���ı���ȡ������ջ��,���Ϊl
        case LODT:
            l=p_code[pc].par1.zheng;

            if(l==0){
                if(mem[run[rtop].zheng].isfloat){
                    run[rtop].isfloat=1;
                    run[rtop].shi=mem[run[rtop].zheng].shi;
                }
                else{
                    run[rtop].isfloat=0;
                    run[rtop].zheng=mem[run[rtop].zheng].zheng;
                }
            }
            else{
                if(mem[run[rtop].zheng+mbase].isfloat){
                    run[rtop].isfloat=1;
                    run[rtop].shi=mem[run[rtop].zheng+mbase].shi;
                }
                else{
                    run[rtop].isfloat=0;
                    run[rtop].zheng=mem[run[rtop].zheng+mbase].zheng;
                }
            }
            pc++;
            break;
        //LOA l A,�����Ϊl,��Ե�ַΪA�ĵ�ַȡ������ջ��
        case LOA:
            rtop++;
            l=p_code[pc].par1.zheng;
            A=p_code[pc].par2.zheng;
            if(l==0){
                run[rtop].isfloat=0;
                run[rtop].zheng=A;
            }
            else{
                run[rtop].isfloat=0;
                run[rtop].zheng=A+mbase;
            }
            pc++;
            break;
         //STT l 0,��ջ�������ڴ�ջ����ַ��
        case STT:


        if(p_code[pc].par2.zheng==1){
            if(run[rtop].isfloat){
            mem[run[rtop-1].zheng].isfloat=1;
            mem[run[rtop-1].zheng].shi=run[rtop].shi;
            }
            else{
            mem[run[rtop-1].zheng].isfloat=1;
            mem[run[rtop-1].zheng].shi=run[rtop].zheng;
        }
        }
        else{
            if(run[rtop].isfloat){
            mem[run[rtop-1].zheng].isfloat=1;
            mem[run[rtop-1].zheng].shi=run[rtop].shi;
            }
            else{
            mem[run[rtop-1].zheng].isfloat=0;
            mem[run[rtop-1].zheng].zheng=run[rtop].zheng;
        }
        }



            rtop-=2;
            pc++;
            break;
        //LOG 0 A,�߼����㼯�ϣ����д�ջ����ջ�����߼����� ������ڴ�ջ��
        //˳������Ϊ1 > 2 >= 3 < 4 <= 5 != 6 ==
        case LOG:

            switch(p_code[pc].par2.zheng){
            case 1:
                if(run[rtop].isfloat==0&&run[rtop-1].isfloat==0)run[rtop-1].zheng=run[rtop-1].zheng>run[rtop].zheng;
                else if(run[rtop].isfloat==1&&run[rtop-1].isfloat==0)run[rtop-1].zheng=run[rtop-1].zheng>run[rtop].shi;
                else if(run[rtop].isfloat==0&&run[rtop-1].isfloat==1){
                        run[rtop-1].isfloat=0;
                        run[rtop-1].zheng=run[rtop-1].shi>run[rtop].zheng;
                }
                else if(run[rtop].isfloat==1&&run[rtop-1].isfloat==1){
                        run[rtop-1].isfloat=0;
                        run[rtop-1].zheng=run[rtop-1].shi>run[rtop].shi;
                }
                break;
            case 2:
                if(run[rtop].isfloat==0&&run[rtop-1].isfloat==0)run[rtop-1].zheng=run[rtop-1].zheng>=run[rtop].zheng;
                else if(run[rtop].isfloat==1&&run[rtop-1].isfloat==0)run[rtop-1].zheng=run[rtop-1].zheng>=run[rtop].shi;
                else if(run[rtop].isfloat==0&&run[rtop-1].isfloat==1){
                        run[rtop-1].isfloat=0;
                        run[rtop-1].zheng=run[rtop-1].shi>=run[rtop].zheng;
                }
                else if(run[rtop].isfloat==1&&run[rtop-1].isfloat==1){
                        run[rtop-1].isfloat=0;
                        run[rtop-1].zheng=run[rtop-1].shi>=run[rtop].shi;
                }
                break;
            case 3:
                if(run[rtop].isfloat==0&&run[rtop-1].isfloat==0)run[rtop-1].zheng=run[rtop-1].zheng<run[rtop].zheng;
                else if(run[rtop].isfloat==1&&run[rtop-1].isfloat==0)run[rtop-1].zheng=run[rtop-1].zheng<run[rtop].shi;
                else if(run[rtop].isfloat==0&&run[rtop-1].isfloat==1){
                        run[rtop-1].isfloat=0;
                        run[rtop-1].zheng=run[rtop-1].shi<run[rtop].zheng;
                }
                else if(run[rtop].isfloat==1&&run[rtop-1].isfloat==1){
                        run[rtop-1].isfloat=0;
                        run[rtop-1].zheng=run[rtop-1].shi<run[rtop].shi;
                }
                break;
            case 4:
                if(run[rtop].isfloat==0&&run[rtop-1].isfloat==0)run[rtop-1].zheng=run[rtop-1].zheng<=run[rtop].zheng;
                else if(run[rtop].isfloat==1&&run[rtop-1].isfloat==0)run[rtop-1].zheng=run[rtop-1].zheng<=run[rtop].shi;
                else if(run[rtop].isfloat==0&&run[rtop-1].isfloat==1){
                        run[rtop-1].isfloat=0;
                        run[rtop-1].zheng=run[rtop-1].shi<=run[rtop].zheng;
                }
                else if(run[rtop].isfloat==1&&run[rtop-1].isfloat==1){
                        run[rtop-1].isfloat=0;
                        run[rtop-1].zheng=run[rtop-1].shi<=run[rtop].shi;
                }
                break;
            case 5:
                if(run[rtop].isfloat==0&&run[rtop-1].isfloat==0)run[rtop-1].zheng=run[rtop-1].zheng!=run[rtop].zheng;
                else if(run[rtop].isfloat==1&&run[rtop-1].isfloat==0)run[rtop-1].zheng=run[rtop-1].zheng!=run[rtop].shi;
                else if(run[rtop].isfloat==0&&run[rtop-1].isfloat==1){
                        run[rtop-1].isfloat=0;
                        run[rtop-1].zheng=run[rtop-1].shi!=run[rtop].zheng;
                }
                else if(run[rtop].isfloat==1&&run[rtop-1].isfloat==1){
                        run[rtop-1].isfloat=0;
                        run[rtop-1].zheng=run[rtop-1].shi!=run[rtop].shi;
                }
                break;
            case 6:
                if(run[rtop].isfloat==0&&run[rtop-1].isfloat==0)run[rtop-1].zheng=run[rtop-1].zheng==run[rtop].zheng;
                else if(run[rtop].isfloat==1&&run[rtop-1].isfloat==0)run[rtop-1].zheng=run[rtop-1].zheng==run[rtop].shi;
                else if(run[rtop].isfloat==0&&run[rtop-1].isfloat==1){
                        run[rtop-1].isfloat=0;
                        run[rtop-1].zheng=run[rtop-1].shi==run[rtop].zheng;
                }
                else if(run[rtop].isfloat==1&&run[rtop-1].isfloat==1){
                        run[rtop-1].isfloat=0;
                        run[rtop-1].zheng=run[rtop-1].shi==run[rtop].shi;
                }
                break;
            }
            rtop--;
            pc++;
            break;
        //LOGT 0 A�߼����㼯�ϣ����д�ջ����ջ�����߼����� �������ջ��
        //˳������Ϊ1 > 2 >= 3 < 4 <= 5 != 6 ==
        case LOGT:
            if(run[rtop].isfloat||run[rtop-1].isfloat)error(34);
            switch(p_code[pc].par2.zheng){
            case 1:
                run[rtop].zheng=run[rtop-1].zheng>run[rtop].zheng;
                break;
            case 2:
                run[rtop].zheng=run[rtop-1].zheng>=run[rtop].zheng;
                break;
            case 3:
                run[rtop].zheng=run[rtop-1].zheng<run[rtop].zheng;
                break;
            case 4:
                run[rtop].zheng=run[rtop-1].zheng<=run[rtop].zheng;
                break;
            case 5:
                run[rtop].zheng=run[rtop-1].zheng!=run[rtop].zheng;
                break;
            case 6:
                run[rtop].zheng=run[rtop-1].zheng==run[rtop].zheng;
                break;
            }
            pc++;
            break;
        //JPC 0 A,��ջ��Ϊ0 ����ת��ָ��A
        case JPC:
             if(run[rtop].isfloat){
                if(run[rtop].shi==0){
                    pc=p_code[pc].par2.zheng;
                }
                else{
                    pc++;
                }
             }
             else{
                if(run[rtop].zheng==0){
                    pc=p_code[pc].par2.zheng;
                }
                else{
                    pc++;
                }
             }

             rtop--;
             break;
        //JMP 0 A,��������ת��A
        case JMP:
            pc=p_code[pc].par2.zheng;
            break;
        //INT i A,�洢ջջ������A���洢��Ԫ����Щ�洢��Ԫ��Ϊ���ű���i����׼����
        case INT:

            mem[mtop+1].isfloat=0;
            mem[mtop+1].zheng=mbase;
            mbase=mtop;
            mtop=mtop+p_code[pc].par2.zheng;
            mem[mbase+2].isfloat=0;
            if(tab[p_code[pc].par1.zheng].type==1){mem[mbase+3].isfloat=1;}
            else{mem[mbase+3].isfloat=0;}
            pc++;
            break;
        //STI l A,��������l������Ե�ַA��
        case STI:
            mem[mbase+p_code[pc].par2.zheng].isfloat=0;
            mem[mbase+p_code[pc].par2.zheng].zheng=p_code[pc].par1.zheng;
            pc++;
            break;
        //JMB 0 0,���������ret addr��
        case JMB:
            pc=mem[mbase+2].zheng;
            break;
        //STO l A,��ջ��Ԫ�ش��ڲ��Ϊl����Ե�ַΪA�Ĵ洢��
        case STO:
            l=p_code[pc].par1.zheng;
            A=p_code[pc].par2.zheng;
            if(A!=3){
            if(l==0){
                if(run[rtop].isfloat){
                    mem[A].isfloat=1;
                    mem[A].shi=run[rtop].shi;
                }
                else{
                    mem[A].isfloat=0;
                    mem[A].zheng=run[rtop].zheng;
                }
            }
            else{
                if(run[rtop].isfloat){
                    mem[A+mbase].isfloat=1;
                    mem[A+mbase].shi=run[rtop].shi;
                }
                else{
                    mem[A+mbase].isfloat=0;
                    mem[A+mbase].zheng=run[rtop].zheng;
                }
            }
            }
            else{
                if(l==0){
                    if(run[rtop].isfloat==mem[A].isfloat){
                        if(run[rtop].isfloat){
                        mem[A].shi=run[rtop].shi;
                    }
                    else{
                        mem[A].zheng=run[rtop].zheng;
                    }

                    }
                    else if(mem[A].isfloat==1){

                            mem[A].shi=run[rtop].zheng;

                    }
                    else{
                        error(35);
                    }
                }
                else{
                    if(run[rtop].isfloat==mem[A+mbase].isfloat){
                        if(run[rtop].isfloat){
                        mem[A+mbase].shi=run[rtop].shi;
                    }
                    else{
                        mem[A+mbase].zheng=run[rtop].zheng;
                    }
                    }
                    else if(mem[A+mbase].isfloat==1){
                        mem[A+mbase].shi=run[rtop].zheng;
                    }
                    else{
                        error(35);
                    }
                }
            }
            pc++;
            rtop--;
            break;
        //QUT 0 0,�洢ջ��ջһ��
        case QUT:
            mtop=mbase;
            mbase=mem[mbase+1].zheng;
            pc++;
            break;
         //rei 0 0 ��ȡָ��ӱ�׼�����ȡ��������ջ��
        case rei:
            rtop++;
            run[rtop].isfloat=0;
            scanf("%d",&run[rtop].zheng);
            pc++;
            break;
        case ref:
            rtop++;
            run[rtop].isfloat=1;
            scanf("%lf",&run[rtop].shi);
            pc++;
            break;
        case rec:
            rtop++;
            run[rtop].isfloat=0;
            scanf("%c",&run[rtop].zheng);
            pc++;
            break;
        //wr 0 0��׼���ָ����ջ������׼���
        case wr:
            if(p_code[pc].par2.zheng==2){
                if(run[rtop].isfloat){
                    printf("%lf",run[rtop].shi);
                }
                else{
                     printf("%lf",(double)run[rtop].zheng);
                }
            }
            else{
                if(p_code[pc].par2.zheng==1){
                    printf("%d",run[rtop].zheng);
                }
                else if(p_code[pc].par2.zheng==0){
                    putchar(run[rtop].zheng);
                }

            }
            rtop--;
            pc++;
            break;
        case ws:
            A=p_code[pc].par2.zheng;
            printf(hc[A]);
            pc++;
            break;
        }
    }
}
int main()
{
    int x=0;
    int i=0;
    char C0_address[100];
    puts("������Դ����·����");
    gets(C0_address);

    if((C0_code=fopen(C0_address,"r"))==NULL){
        puts("C0�����ļ�λ������");
        exit(1);
    }
    program();
    //for(i=0;i<p_top;i++){
    //      printf("%d: %s,%d,%d,,,%d\n",i,P_code_name_name[p_code[i].id],p_code[i].par1.zheng,p_code[i].par2.zheng,p_code[i].isfloat);
    //}
    //for(i=1;i<=tabtop;i++){
    //    printf("%s,%d,%d\n",tab[i].name,tab[i].kind,tab[i].type);
    //}
    puts("\n�������!��");
    if(is_error==0){
        interupt();
    }

}
