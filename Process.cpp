//This is Process.cpp
#include "Process.hpp"
#include <sys/wait.h>

Process::Process(const std::vector<char*>& args, bool verbose)
{
    if (fork() == 0)
    {
	char** argv = new char* [args.size()];
	copy(args.begin(), args.end(), argv);
	execvp(argv[0], argv);
    }
}

Process::~Process()
{
	if (waitpid((pid_t)-1, NULL, 0) > 0){};
}
