#include <iostream>
#include <cstring>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <pwd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include "Parser.h"
using namespace std;

struct Context {
    int PID;
    char **argv;
    struct Context *next;
};

class Interpreter
{
int stdinFD, stdoutFD;      /* сохранённые дескрипторы стандартных IO-потоков */
char *HOME;                 /* домашняя директория пользователя */
char WD[PATH_MAX+1];        /* рабочая директория */
char SHELL[PATH_MAX+1];     /* путь к исполняемому в данный момент шеллу */
char *USER;                 /* имя пользователя, запустившего процесс */
unsigned long EUID;         /* идентификатор пользователя, с правами которого
                             работает процесс */
public:
static void sigintHandler(int sig) ;/* Обработчик сигнала SIGINT */
void addBack(Context **c, int PID, char **argv) ;/* Функция добавляет процесс к списку фоновых процессов */
void clearBack(Context **c, int waitExit) ;/* Функция проверяет список фоновых процессов и выводит информацию о завершившихся *//*该函数检查后台进程的列表，并显示已完成的信息。*/
void killBack(Context *c);
int run(RunTree *RT, Context **BP);
void star_myshell(int argc, char **argv, char **envp);

};
