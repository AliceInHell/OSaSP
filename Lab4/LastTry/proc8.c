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

void Proc8(int signo, siginfo_t *siginfo, void *context)
{
	//usleep(100000);
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC7 PUT SIGUSR1\n", msg, getpid(), getppid(), getCurrentTime());
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC8 GET SIGUSR1 FROM %d\n", msg++, getpid(), getppid(), getCurrentTime(), siginfo->si_pid);
	usleep(10000);
	kill(getpid() - 7, SIGUSR2);
}

int main(void)
{
	struct sigaction sa8;
	memset(&sa8, '\0', sizeof(sa8));
	sa8.sa_sigaction = &Proc8;
	sa8.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sa8, NULL);

  	for (;;)
		pause();			
	exit(0);
}
