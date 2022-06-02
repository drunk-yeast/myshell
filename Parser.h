#include "Lex.h"
struct RunTree {
    //vector<string> argv;/* список из имени команды и аргументов */
    char ** argv;
    string infile;
    string outfile;
    int append;
    int backgrnd;
    int logical;
    RunTree* psubcmd;
    RunTree* pipe;
    RunTree* next;
};


class Parser
{
	Lex obj;
	deque<string>& lst = obj.lst;
	static string c;
public:
    int getlist();
    int getShellCommand(RunTree** RT);//P
    int getCommandList(RunTree** RT);//Q
    int getPipeline(RunTree** RT);//R
    int getCommand(RunTree** RT);//S
    int getSimpleCommand(RunTree** RT);//T
    int getIO(RunTree** RT);
    void getArguments(RunTree** RT);
    void delRunTree(RunTree** RT);
    void variablesChg(char*HOME, char*SHELL, char*USER, unsigned long EUID);
    int empty() {
        return lst.empty();
    }
};
#if 0
< команда_shell > :: = [<список_команд>]['#' < комментарий > ]
<список_команд>   :: = <конвейер>{ [один из '&', ';', '&&', '||'] <конвейер> }[один из '&', ';']
<конвейер>        :: = <команда>{ '|' < команда > }
<команда>         :: = <простая_команда> | '(' < список_команд > ')' { [один из '>', '>>', '<'] <имя_файла> }
<простая_команда> :: = <имя_файла>{ <аргумент> } { [один из '>', '>>', '<'] <имя_файла> }
#endif
