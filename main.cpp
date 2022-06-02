#include <iostream>
#include "Interpreter.h"
using namespace std;

int main(int argc, char **argv, char **envp)
{
    Interpreter ip;
    ip.star_myshell(argc, argv, envp);
}
