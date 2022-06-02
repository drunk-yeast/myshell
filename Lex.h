#include "iostream"
#include "deque"
#include "cstring"
using namespace std;
class Lex
{
    char ___buf[BUFSIZ + 1];
    int ___free_pointer = BUFSIZ;
    string tmp;
    deque<string> lst;
public:
    friend class Parser;
    int getbuf(void) ;
    void ungetbuf(int c);
    void cleanbuf(void) ;
    void showlist() ;
    int getword(void) ;
    int getlist() ;
};

