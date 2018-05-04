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

void Proc6(int signo, siginfo_t *siginfo, void *context)
{
	usleep(100000);
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC4 PUT SIGUSR1\n", msg, getpid(), getppid(), getCurrentTime());
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC6 GET SIGUSR1 FROM %d\n", msg++, getpid(), getppid(), getCurrentTime(), siginfo->si_pid);
}

int main(void)
{
	struct sigaction sa6;
	memset(&sa6, '\0', sizeof(sa6));
	sa6.sa_sigaction = &Proc6;
	sa6.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sa6, NULL);
  	
	pid_t pid, tpid;
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
			execl("./proc7.out", "./proc7.out", NULL);
			perror("error execl");
			exit(-1);
		}
		default:
		{
			tpid = fork();
			switch (tpid)
			{
				case -1:
					perror("error");
					break;
				case 0:
					execl("./proc8.out", "./proc8.out", NULL);
					perror("error execl");
					break;
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
			
			int st = 0;
			wait(&st);
			break;	
		}
	}
	return 0;
}
