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

void Proc1(int signo, siginfo_t *siginfo, void *context)
{
	//usleep(100000);
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC8 PUT SIGUSR2\n", msg, getpid(), getppid(), getCurrentTime());
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC1 GET SIGUSR2 FROM %d\n", msg++, getpid(), getppid(), getCurrentTime(), siginfo->si_pid);
	usleep(100000);
	kill(getpid() + 1, SIGUSR1);
	usleep(100000);
	kill(getpid() + 2, SIGUSR1);
}

int main(void)
{
	struct sigaction sa1;
	memset(&sa1, '\0', sizeof(sa1));
	sa1.sa_sigaction = &Proc1;
	sa1.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR2, &sa1, NULL);
  	
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
			execl("./proc2.out", "./proc2.out", NULL);
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
					execl("./proc3.out", "./proc3.out", NULL);
					perror("error execl");
					break;
				default:
				{
					sleep(2);
					kill(getpid() + 1, SIGUSR1);
					kill(getpid() + 2, SIGUSR1);
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
