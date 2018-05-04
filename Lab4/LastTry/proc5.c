#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <math.h>

int msg = 0;

int getCurrentTime()
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int timeInMilliseconds = (int)(ts.tv_nsec/1000000);
	return timeInMilliseconds;
}

void Proc5(int signo, siginfo_t *siginfo, void *context)
{
	//usleep(100000);
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC4 PUT SIGUSR1\n", msg, getpid(), getppid(), getCurrentTime());
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC5 GET SIGUSR1 FROM %d\n", msg++, getpid(), getppid(), getCurrentTime(), siginfo->si_pid);
}

int main(void)
{
	struct sigaction sa5;
	memset(&sa5, '\0', sizeof(sa5));
	sa5.sa_sigaction = &Proc5;
	sa5.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sa5, NULL);
  	
	pid_t pid;
	pid = fork();
	switch (pid)
	{
		case -1:
		{
			perror("error");
			break;
		}
		case 0:
		{			
			execl("./proc6.out", "./proc6.out", NULL);
			perror("error execl");
			exit(-1);
		}
		default:
		{	
			for (;;)
			{
				pause();
			}
					
			int st = 0;
			wait(&st);
			break;	
		}
	}
	return 0;
}
