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

void Proc7(int signo, siginfo_t *siginfo, void *context)
{
	//usleep(100000);
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC4 PUT SIGUSR1\n", msg, getpid(), getppid(), getCurrentTime());
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC7 GET SIGUSR1 FROM %d\n", msg++, getpid(), getppid(), getCurrentTime(), siginfo->si_pid);
	usleep(100000);
	kill(getpid() + 1, SIGUSR1);
}

int main(void)
{
	struct sigaction sa7;
	memset(&sa7, '\0', sizeof(sa7));
	sa7.sa_sigaction = &Proc7;
	sa7.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sa7, NULL);

  	for (;;)
		pause();			
	exit(0);
}
