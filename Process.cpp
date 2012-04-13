//This is Process.cpp
#include "Process.hpp"
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <algorithm>

Process::Process(const std::vector<char*>& args, bool verbose):
	verbose(verbose),
	m_name(args[0]),
	m_pid((pid_t)NULL),
	m_writepipe {-1,-1},
	m_readpipe {-1,-1},
	m_pwrite((FILE*)NULL),
	m_pread((FILE*)NULL)
{
	if (pipe(m_writepipe) < 0 || pipe(m_readpipe) <0)
	{
		perror("pipe");
		throw std::string("pipe");
	}

	m_pid = fork();
    	if (m_pid == 0)
    	{
		if (close(PARENT_READ) < 0 || close(PARENT_WRITE) < 0)
		{
			perror("child process: close parent");
			throw std::string("child process: close parent");
		}

		if (dup2(CHILD_WRITE, 1) < 0)
		{
			perror("child process: dup write");
			throw std::string("child process: dup write");
		}
		if (close(CHILD_WRITE) < 0)
		{
			perror("child process: close write");
			throw std::string("child process: close write");
		}
		if (dup2(CHILD_READ, 0) < 0)
		{
			perror("child process: dup read");
			throw std::string("child process: dup read");
		}
		if (close(CHILD_READ) < 0)
		{
			perror("child process: close read");
			throw std::string("child process: close read");
		}
		char** argv = new char* [args.size()];
		copy(args.begin(), args.end(), argv);
		execvp(argv[0], argv);
		perror("Execvp");
		throw std::string("Execvp");
    	}
    	else if (m_pid < 0)
	{
		perror("Process fork");
		throw std::string("Process fork");
	}
	else
	{
		if (close(CHILD_READ) < 0 || close(CHILD_WRITE) < 0)
		{
			perror("parent process: close child");
			throw std::string("parent process: close child");
		}

		m_pread = fdopen(PARENT_READ, "r");
		m_pwrite = fdopen(PARENT_WRITE, "w");

		if (m_pread == NULL || m_pwrite == NULL)
		{
			perror("parent process: open");
			throw std::string("parent process: open");
		}

	}
}

Process::~Process()
{
	if (fclose(m_pwrite) < 0)
	{
		perror("~process: close write");
	}
	int status;
	pid_t pid = waitpid(m_pid, &status, 0);
	if (pid < 0)
		perror("~Process wait");
	if (fclose(m_pread) < 0)
	{
		perror("~process: close read");
	}
}

void Process::write(const std::string& line)
{
	if (fputs(line.c_str(), m_pwrite) < 0)
	{
		perror("write");
	}
	if (fflush(m_pwrite) < 0)
	{
		perror("write");
	}
}

std::string Process::read()
{
	std::string line;
	char* mystring = NULL;
	size_t num_bytes;

	if (getline(&mystring, &num_bytes, m_pread) < 0)
	{
		perror("read");
	}
	line = mystring;
	return line;
}
