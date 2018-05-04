#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

int main(void)
{
	int st = 0;
	pid_t pid;
	pid = fork();
	switch (pid)
	{
		case -1:
			perror("error");
			break;
		case 0:
			execl("./proc1.out", "./proc1.out", NULL);
			exit(-1);
		default:
			wait(&st);
			break;	
	}
	
	return 0;
}
