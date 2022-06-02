#include "Interpreter.h"

void Interpreter::sigintHandler(int sig) {
    signal(SIGINT, SIG_IGN);
    kill(0,SIGINT);
    cout << endl;
}
void Interpreter::addBack(Context **c, int PID, char **argv) {
    while (*c!=NULL)
        c=&((*c)->next);
    *c=(Context *)malloc(sizeof(Context));
    //*c = new Context;
    (*c)->next=NULL;
    (*c)->PID=PID;
    (*c)->argv=argv;
}
void Interpreter::clearBack(Context **c, int waitExit) {
    Context *tmp;
    int status, ret;
    unsigned long i;
    while (*c!=NULL) {
        ret=waitpid((*c)->PID, &status, waitExit?0:(WNOHANG));
        if (ret) {
            if (ret>0 && waitExit!=3 && (*c)->argv) {
                if (WIFEXITED(status))
                    printf ("Background process [%d] (%s) was exited with exit code %d\n",
                            (*c)->PID,(*c)->argv[0],WEXITSTATUS(status));
                else
                    if (WIFSIGNALED(status))
                        printf ("Background process [%d] (%s) was terminated by signal %d\n",
                                (*c)->PID,(*c)->argv[0],WTERMSIG(status));
                    else
                        printf ("Background process [%d] (%s) isn't alive any more\n",
                                (*c)->PID,(*c)->argv[0]);
                for (i=0; (*c)->argv[i]!=NULL; i++)
                    free((*c)->argv[i]);
            }
            free((*c)->argv);
            tmp=*c;
            *c=(*c)->next;
            free(tmp);
        }
        else
            c=&((*c)->next);
    }
    return;
}
void Interpreter::killBack(Context *c) {
    while (c!=NULL) {
        kill(c->PID, SIGKILL);
        c=c->next;
    }
}
int Interpreter::run(RunTree *RT, Context **BP) {
    Context *PipelineProcs=NULL;
    int localStdinFD=stdinFD, localStdoutFD=stdoutFD;
    int masterPID, PID;
    int tmpFD;
    int pipes[]={0,0};
    int status;
    int back=0;

    masterPID=getpid();
    if (RT==NULL)
        return 0;
    while (1) {
        if (pipes[0]) {
            dup2(pipes[0],0);
            close(pipes[0]);
            pipes[0]=0;
        } else
            if (!RT->infile.empty()) {
                if ((tmpFD=open(RT->infile.c_str(), O_RDONLY, 0))==-1) {
                    fprintf(stderr,
                            "Can't find input file.\n");
                    return -3;
                }
                dup2(tmpFD,0);
                close(tmpFD);
            } else
                if (RT->backgrnd) {
                    tmpFD=open("/dev/null", O_RDONLY, 0);
                    dup2(tmpFD,0);
                    close(tmpFD);
                }
                else
                    dup2(localStdinFD,0);

        if (!RT->outfile.empty()) {
            tmpFD=open(RT->outfile.c_str(),
                       (RT->append)?(O_CREAT | O_APPEND | O_RDWR):(O_CREAT | O_WRONLY | O_TRUNC), 0666);
            dup2(tmpFD,1);
            close(tmpFD);
            if (RT->pipe && RT->pipe->infile.empty()) {
                RT->pipe->infile = "/dev/null";
            }
        } else
            if (RT->pipe) {
                pipe(pipes);
                dup2(pipes[1],1);
                close(pipes[1]);
                pipes[1]=0;
            } else
                dup2(localStdoutFD,1);

        if ((RT->next && RT->pipe) || RT->psubcmd || (RT->argv && strcmp(RT->argv[0],"cd") &&
                                                      strcmp(RT->argv[0],"exit"))) {
            if ((PID=fork())==0) {
                if (pipes[0])
                    close(pipes[0]);
                if (pipes[1])
                    close(pipes[1]);
                pipes[0]=0;
                pipes[1]=0;
                if (RT->next && RT->pipe) {
                    RT->next=NULL;
                    continue;
                }
                if (RT->backgrnd)
                    signal(SIGINT, SIG_IGN);
                else
                    signal(SIGINT, SIG_DFL);
                if (RT->argv && execvp(RT->argv[0], RT->argv)==-1) {
                    fprintf(stderr, "EXEC failed.\n");
                    return -1;
                }
                localStdinFD=dup(0);
                localStdoutFD=dup(1);
                RT=RT->psubcmd;
                continue;
            }
            else
                if (PID==-1) {
                    fprintf(stderr,
                            "Wncorrectable error. FORK failed. Shell exiting now.\n");
                    return -1;
                }
        }

        if (!RT->pipe && RT->argv && !strcmp(RT->argv[0],"cd") && chdir(RT->argv[1]?RT->argv[1]:HOME))
            printf ("Directory changing failed\n");

        if (!RT->pipe && RT->argv && !strcmp(RT->argv[0],"exit"))
            return -1;

        if (RT->next && RT->pipe) {
            RT->pipe=NULL;
            RT->argv=NULL;
        }

        dup2(localStdinFD,0);
        dup2(localStdoutFD,1);

        if (getpid()==masterPID && (RT->backgrnd || back) && (!RT->argv || (RT->argv && strcmp(RT->argv[0],"cd") && strcmp(RT->argv[0],"exit")))) {
            addBack(BP,PID,RT->argv);
            if (RT->pipe)
                back=1;
            RT->argv=NULL;
        }
        else
            if (getpid()==masterPID && (RT->pipe) && !(RT->backgrnd) && !back && (RT->argv && strcmp(RT->argv[0],"cd") && strcmp(RT->argv[0],"exit"))) {
                addBack(&PipelineProcs,PID,RT->argv);
                RT->argv=NULL;
            }
            else
                if ((RT->argv && strcmp(RT->argv[0],"cd")) || !RT->argv)
                    waitpid(PID,&status,0);

        if (RT->pipe) {
            RT=RT->pipe;
            continue;
        } else {
            clearBack(&PipelineProcs,3);
            PipelineProcs=NULL;
            back=0;
            if (RT->next && (RT->logical==0 ||
                             (RT->logical==1 && WIFEXITED(status) && WEXITSTATUS(status)==0) ||
                             (RT->logical==2 && (!WIFEXITED(status) || WEXITSTATUS(status)!=0)))) {
                RT=RT->next;
                continue;
            } else
                break;
        }
    }
    if (getpid()!=masterPID)
        return -2;
    dup2(localStdinFD,0);
    dup2(localStdoutFD,1);
    return 0;
}
void Interpreter::star_myshell(int argc, char **argv, char **envp) {
    RunTree *RT=NULL;
    Context *backProcs=NULL;
    int ret;
    struct passwd *userInfo;
    signal(SIGINT, sigintHandler);
    stdinFD=dup(0);
    stdoutFD=dup(1);
    EUID=geteuid();
    userInfo=getpwuid(EUID);
    USER=userInfo->pw_name;
    HOME=userInfo->pw_dir;
    SHELL[0]=0;
    strcat(SHELL, getenv("PWD"));
    strcat(SHELL, "/");
    strcat(SHELL, &(argv[0][argv[0][0]=='.'?2:0]));
    chdir(HOME);
    Parser parser;
    //cout << "********************* MyShell ************************" << endl;
    do {
        printf ("%s:~%s$ ", USER, getcwd(WD, sizeof(WD)));//getwd->getcwd
        ret=parser.getlist();
        if (ret==0 && parser.empty()) {//正常.空命令,什么也不做.
            clearBack(&backProcs,0);
            continue;
        }
        if (ret==-1) {//getlist异常.
            printf ("Error. Undeclarated symbols are detected\n");
            ret=0;//标记
            continue;
        }
        if (ret==0 && !parser.empty()) {//正常情况.
            parser.variablesChg(HOME, SHELL, USER, EUID);
            if (parser.getShellCommand(&RT)) {//返回0,-1

                printf("Syntaxis error!\n");//返回-1;
                parser.delRunTree (&RT);
            }
            ret=run(RT, &backProcs);//getShellCommand一切正常.

            if (ret==-3)
                kill(0,SIGINT);
            parser.delRunTree (&RT);
            if (ret!=-2)
                clearBack(&backProcs,0);
        }
    } while (ret==0 || ret==-3);

    if (ret==1)
        printf ("\n");
    if (ret!=-2) {
        killBack(backProcs);
        clearBack(&backProcs,1);
    }
}
