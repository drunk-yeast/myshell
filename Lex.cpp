#include "Lex.h"


int Lex::getbuf(void) {//突然觉得这个小函数设计得好妙啊!
    if (___free_pointer == BUFSIZ) {
        if (fgets(___buf, BUFSIZ + 1, stdin) == NULL)//fgets就是读一行!!!遇到换行和EOF才停住,如果什么都没读入就返回null代表失败.
            return EOF;
        ___free_pointer = -1;
        return getbuf();
    }
    if (___buf[++___free_pointer] == 0) {//遇到\0是重置了.
        ___free_pointer = BUFSIZ;
        return getbuf();
    }
    return ___buf[___free_pointer];
}
void Lex::ungetbuf(int c) {
    ___buf[___free_pointer--] = c;//他这儿没有区别int 与 char
}
void Lex::cleanbuf(void) {
    int c;
    while ((c = getbuf()) != '\n' && c != EOF);
}
void Lex::showlist(){
    for (deque<string>::iterator it = lst.begin(); it != lst.end(); it++) {
        cout << *it << endl;
    }
}
int Lex::getword(void) {
    int c;
    while ((c = getbuf()) == ' ' || c == '\t');
    if (c == '\n')
        return 0;//0是正常
    if (c == EOF)
        return 1;//1与0 有点不同
    switch (c) {
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
    case 'V': case 'W': case 'X': case 'Y': case 'Z':

    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
    case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
    case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
    case 'v': case 'w': case 'x': case 'y': case 'z':

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':

    case '_': case '/': case '$': case '-': case '+': case '*': case '.': {
        tmp += c;
        c = getbuf();
        goto SI;
    }

    case '|': {
        tmp += c;
        c = getbuf();
        goto AO;
    }
    case '&': {
        tmp += c;
        c = getbuf();
        goto AA;
    }
    case '>': {
        tmp += c;
        c = getbuf();
        goto AM;
    }
    case ';': case '<': case '#': case '(': case ')': {
        tmp += c;
        c = getbuf();
        goto WS;
    }
    default: {//如果你第一个字符都乱输入 例如: :{}~什么的就直接清空退出啊
        return -1;
    }
    }
    SI: switch (c) {
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
    case 'V': case 'W': case 'X': case 'Y': case 'Z':

    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
    case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
    case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
    case 'v': case 'w': case 'x': case 'y': case 'z':

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':

    case '_': case '/': case '$': case '-': case '+': case '*': case '.': {
        tmp += c;
        c = getbuf();
        goto SI;
    }
    default:
        goto EX;
    }
    AO: switch (c) {
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
    case 'V': case 'W': case 'X': case 'Y': case 'Z':

    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
    case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
    case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
    case 'v': case 'w': case 'x': case 'y': case 'z':

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':

    case '_': case '/': case '$': case '-': case '+': case '*': case '.':

    case '&': case '>': case ';': case '<': case '#': case '(': case ')':

    case ' ': case '\t': case '\n': case EOF:
        goto EX;
    case '|': {
        tmp += c;
        c = getbuf();
        goto WS;
    }
    default: {
        return -1;
    }
    }
    AA: switch (c) {
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
    case 'V': case 'W': case 'X': case 'Y': case 'Z':

    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
    case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
    case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
    case 'v': case 'w': case 'x': case 'y': case 'z':

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':

    case '_': case '/': case '$': case '-': case '+': case '*': case '.':

    case '|': case '>': case ';': case '<': case '#': case '(': case ')':

    case ' ': case '\t': case '\n': case EOF:
        goto EX;
    case '&': {
        tmp += c;
        c = getbuf();
        goto WS;
    }
    default: {
        return -1;
    }
    }
    AM: switch (c) {
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
    case 'V': case 'W': case 'X': case 'Y': case 'Z':

    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
    case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
    case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
    case 'v': case 'w': case 'x': case 'y': case 'z':

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':

    case '_': case '/': case '$': case '-': case '+': case '*': case '.':

    case '&': case '|': case ';': case '<': case '#': case '(': case ')':

    case ' ': case '\t': case '\n': case EOF:
        goto EX;
    case '>': {
        tmp += c;
        c = getbuf();
        goto WS;
    }
    default: {
        return -1;
    }
    }
    WS: switch (c) {
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
    case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
    case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
    case 'V': case 'W': case 'X': case 'Y': case 'Z':

    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
    case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
    case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
    case 'v': case 'w': case 'x': case 'y': case 'z':

    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':

    case '_': case '/': case '$': case '-': case '+': case '*': case '.':

    case '&': case '|': case '>': case ';': case '<': case '#': case '(': case ')':

    case ' ': case '\t': case '\n': case EOF:
        goto EX;
    default: {
        return -1;
    }
    }
    EX:	ungetbuf(c);
    return 0;
}
int Lex::getlist() {
    int ret;
    while (!(ret = getword()) && !tmp.empty())//0表示正常,-1表示异常.1表示到达文件末尾. tmp非空
    {
        lst.push_back(tmp);
        tmp.clear();
    }
    if (ret == -1) {
        cleanbuf();
    }
    return ret;
}
