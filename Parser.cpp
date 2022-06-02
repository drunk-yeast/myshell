#include "Parser.h"
#include <cstring>

int Parser::getlist() {
    int ret;
    ret = obj.getlist();
    //obj.showlist();
    return ret;
}

int Parser::getShellCommand(RunTree** RT) {
    int ret = 0;
    string tmp;
    //Pbeg:
    if(!lst.empty()) {
        //Qbeg:
        if (getCommandList(RT)) ret=  -1;//如果getCommanList失败或者getCommandList成功但是第一个lex不是#,那么ret-1 表示失败,因为#表示注释!!
        //Qend:
        //P1:
        if (!lst.empty()) {
            tmp = lst.front();
            lst.pop_front();
            //P2:
            if (!lst.empty() && tmp != "#") ret = -1; //注意 compare 相等返回0!!
            //lst.pop_front();
            lst.clear();
        }
    }
    //Pend:
    return ret;
}
int Parser::getCommandList(RunTree **RT) {
    string tmp1, tmp2;
    //Qbeg:
    while (1) {
        //Rbeg:
        if (getPipeline(RT))//返回0表示正常,返回-1表示异常.
            return -1;
        //Rend:
        if(lst.empty())
            return 0;//如果空命令直接回车,也是正常返回.
        tmp1 = lst.front();
        lst.pop_front();
        //Q1
        if(tmp1 == "&&" || tmp1 == "||") {
            if (tmp1 == "&&"){
                (*RT)->logical = 1;//logical的含义!!! append表示 >>
                /* 0 - безусловное выполнение следующей команды (;)
                1 - выполнение в случае успешного завершения (&&)
                2 - выполнение в случае неуспешного завершения (||) */
                RT = &((*RT)->next);//就不太了解这一步?????大概是构造了RT,现在要去构造next的RT了.
                //Qbeg:
                continue;//continue 继续getPipeline
            }
            else
                if (tmp1 == "||") {
                    (*RT)->logical = 2;//logical的含义!!! append表示 >>
                    RT = &((*RT)->next);
                    //Qbeg:
                    continue;//continue 继续getPipeline
                }
        }
        //Q2
        else if(tmp1 == "&" || tmp1 == ";") {
            if (tmp1 == "&")
                (*RT)->backgrnd = 1;//表示在后台运行.
            else if(tmp1 == "&") ;
        }
        else if (tmp1 == "#" || tmp1 == ")") {
            //不清楚为什么会有这样的情况? wow,如下,好细节啊这程序.
            //'('<список_команд>')'
            //[<список_команд>] ['#' < комментарий > ]
            lst.push_front(tmp1);
            return 0;
        }
        else return -1;// && || & ; # )

        //Qend:
        if (lst.empty())//能走到这里,只能是因为碰到了&后台执行, ;
            return 0;//& ;之后就没了,那么正常退出.
        tmp2 = lst.front();
        if (tmp2 == "#" || tmp2 == ")")
            return 0;//同样参考前面那儿,#,)可能出现在getcommandLost之后,即也可能出现在&之后.
        //Qbeg:
        RT = &((*RT)->next);//这是因为还要继续构造RunTree, 大循环是while(1);
    }
}
int Parser::getPipeline(RunTree** RT) {//它就是<конвейер>
    int ret;
    string tmp;
    //Rbeg:
    while (1) {
        //Sbeg:
        ret = getCommand(RT);
        //Send:
        if(ret) return -1;

         //R1:
        if (lst.empty())//如果没有 |, 正常返回.
            return 0;
        tmp = lst.front();
        lst.pop_front();
        //Rend:
        if (tmp != "|") {//如果不是 |, 正常返回.
            lst.push_front(tmp);
            return 0;
        }
        //Qbeg:
        else RT = &((*RT)->pipe);//如果有 |, 其实调用getPipeline时还没有RunTree,但前面if (getCommand(RT, WL))后构造了RunTree.
                            //struct RunTree* pipe; 所以这指针就很细节.
    }
}
int Parser::getCommand(RunTree** RT) {
    int ret;
    string tmp;
    //Sbeg:
    if (lst.empty())
        return -1;
    tmp = lst.front();
    lst.pop_front();

    //Tbeg:
    if (tmp != "(") { //注意 0表示相等.
        lst.push_front(tmp);// ls -l < f1 > f2 #here: ls != '(' so push_front
        //Tbeg:
        ret = getSimpleCommand(RT);
        //Tend:
        return ret;
    }

    //<команда>         ::= <простая_команда> | '('<список_команд>')' { [один из '>', '>>', '<'] <имя_файла> }
    //小细节 这儿用的是 | ,看getCommand是否以(开头.
    //free(tmp);//如果是 ( 那就不执行getsimplecommand.
    //*RT = (RunTree*) malloc(sizeof(RunTree));//同样的要构造RunTree;

    //S1:
    *RT = new RunTree;
    (*RT)->argv = NULL;
    (*RT)->next = NULL;
    (*RT)->pipe = NULL;
    (*RT)->append = 0;//下面getsimplecommand可知, append表示>>;
    (*RT)->backgrnd = 0;
    (*RT)->logical = 0;
    (*RT)->psubcmd = NULL;
    //Qbeg:
    ret = getCommandList(&((*RT)->psubcmd));
    //Qend:
    if(ret) return -1;

    //S2:
    if (lst.empty())//getCommandList后不能是空,还要有)呢;
        return -1;
    tmp = lst.front();
    lst.pop_front();
    //S3:
    if (tmp.compare(")")) {//getCommandList后不能是空,还要有)呢
        return -1;
    }
    //getIO:
    ret = getIO(RT);
    //Send:
    return ret;//getIO其实就是构造RunTree,构造 < > >> 这种结构.

}
int Parser::getSimpleCommand(RunTree** RT) {
    //*RT = malloc(sizeof(RunTree));//这儿才开始构造RunTree!!!
    *RT = new RunTree;
    (*RT)->argv = NULL;
    (*RT)->next = NULL;
    (*RT)->pipe = NULL;
    //(*RT)->infile = NULL;
    //(*RT)->outfile = NULL;
    (*RT)->append = 0;
    (*RT)->backgrnd = 0;
    (*RT)->logical = 0;
    (*RT)->psubcmd = NULL;
    getArguments(RT);//一直添加,直到遇到 > >> ; &等等符号.
    if ((*RT)->argv==NULL)
        return -1;
    return getIO(RT);
}
int Parser::getIO(RunTree** RT) {
    string tmp;
    int i;
    for (i = 0; i < 2; i++) {//为什么两边呢?因为有I 也有 O
        if (lst.empty())
            return 0;
        tmp = lst.front();
        lst.pop_front();
        if (tmp == ">" || tmp == ">>") {
            if (tmp == ">>")
                (*RT)->append = 1;// >> append = 1;
            if (lst.empty()) return -1;
            tmp = lst.front();
            lst.pop_front();
            if (!(*RT)->outfile.empty()) return -1;
            (*RT)->outfile = tmp;//能走到这儿说明:有lex,且有outfile;
        }
        else
            if (tmp == "<") {
                if (lst.empty()) return -1;
                tmp = lst.front();
                lst.pop_front();
                if (!(*RT)->outfile.empty()) return -1;
                (*RT)->infile = tmp;//能走到这儿说明:有lex,且有infile;
            }
            else
                lst.push_front(tmp);
    }
    return 0;
}
void Parser::getArguments(RunTree** RT) {
    string tmp;
    unsigned long i = 0;
    while (1) {
        if (lst.empty())
            return;
        tmp = lst.front();
        lst.pop_front();
        if( tmp == ">"  || tmp == ">>" || tmp == ";" || tmp == "("  || tmp == ")" ||
            tmp == "&"  || tmp == "&&" || tmp == "|" || tmp == "||" || tmp == "<" || tmp == "#")
        {
            lst.push_front(tmp);
            return;
        }

        //(*RT)->argv.push_back(tmp); !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //here i make a big mistake. at first i use vecort<string> argv, but it's not good, then use char** to replace it; thus
        (*RT)->argv = (char**)realloc((*RT)->argv, (i + 2) * sizeof(char*));//i+2细节.
        char*tmp2 = (char*) malloc(sizeof(char)*(tmp.size()+2));
        strcpy(tmp2, tmp.c_str());
        ((*RT)->argv)[i] = tmp2;//如果不是以上字符的话,就添加tmp到argv.
        ((*RT)->argv)[i + 1] = NULL;
        i++;
    }
}
void Parser::delRunTree(RunTree** RT) {
    if (*RT != NULL) {

        if ((*RT)->argv!=NULL) {
            for (unsigned long i=0; (*RT)->argv[i]!=NULL; i++)
                free((*RT)->argv[i]);
            free((*RT)->argv);
            (*RT)->argv=NULL;
        }
        delRunTree(&((*RT)->next));
        delRunTree(&((*RT)->pipe));
        delRunTree(&((*RT)->psubcmd));
        delete(*RT);
        *RT = NULL;
    }
}
void Parser::variablesChg(char*HOME, char*SHELL, char*USER, unsigned long EUID) {
    for(size_t i = 0; i < lst.size(); i++) {
        char EUIDstr[35];
        sprintf(EUIDstr, "%lu", EUID);
        if(lst[i] == "$HOME") lst[i] = HOME;
        else if(lst[i] == "$SHELL") lst[i] = SHELL;
        else if(lst[i] == "$USER") lst[i] = USER;
        else if(lst[i] == "$EUID") lst[i] = EUIDstr;
    }
}
