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

void Proc4(int signo, siginfo_t *siginfo, void *context)
{
	//usleep(100000);
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC3 PUT SIGUSR2\n", msg, getpid(), getppid(), getCurrentTime());
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC4 GET SIGUSR2 FROM %d\n", msg++, getpid(), getppid(), getCurrentTime(), siginfo->si_pid);
	usleep(100000);
	kill(getpid() + 1, SIGUSR1);
	usleep(100000);
	kill(getpid() + 2, SIGUSR1);
	usleep(100000);
	kill(getpid() + 3, SIGUSR1);
}

int main(void)
{
	struct sigaction sa4;
	memset(&sa4, '\0', sizeof(sa4));
	sa4.sa_sigaction = &Proc4;
	sa4.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR2, &sa4, NULL);

  	for (;;)
		pause();			
	exit(0);
}
