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

void Proc3(int signo, siginfo_t *siginfo, void *context)
{
	//usleep(100000);
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC1 PUT SIGUSR1\n", msg, getpid(), getppid(), getCurrentTime());
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC3 GET SIGUSR1 FROM %d\n", msg++, getpid(), getppid(), getCurrentTime(), siginfo->si_pid);
	usleep(100000);
	kill(getpid() + 1, SIGUSR2);
}

int main(void)
{
	struct sigaction sa3;
	memset(&sa3, '\0', sizeof(sa3));
	sa3.sa_sigaction = &Proc3;
	sa3.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sa3, NULL);
  	
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
			execl("./proc4.out", "./proc4.out", NULL);
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
					execl("./proc5.out", "./proc5.out", NULL);
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
