#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <math.h>

//global
pid_t process1, process2, process3, process4, process5, process6, process7, process8;
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
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC8 PUT SIGUSR2\n", msg++, getpid(), getppid(), getCurrentTime());
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC1 GET SIGUSR2 FROM %d\n", msg++, getpid(), getppid(), getCurrentTime(), siginfo->si_pid);
	usleep(100000);
	kill(process2, SIGUSR1);
	usleep(100000);
	kill(process3, SIGUSR1);
}

void Proc2(int signo, siginfo_t *siginfo, void *context)
{
	//usleep(100000);
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC1 PUT SIGUSR1\n", msg, getpid(), getppid(), getCurrentTime());
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC2 GET SIGUSR1 FROM %d\n", msg++, getpid(), getppid(), getCurrentTime(), siginfo->si_pid);
}

void Proc3(int signo, siginfo_t *siginfo, void *context)
{
	//usleep(100000);
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC1 PUT SIGUSR1\n", msg, getpid(), getppid(), getCurrentTime());
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC3 GET SIGUSR1 FROM %d\n", msg++, getpid(), getppid(), getCurrentTime(), siginfo->si_pid);
	usleep(100000);
	kill(process4, SIGUSR2);
}

void Proc4(int signo, siginfo_t *siginfo, void *context)
{
	//usleep(100000);
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC3 PUT SIGUSR2\n", msg, getpid(), getppid(), getCurrentTime());
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC4 GET SIGUSR2 FROM %d\n", msg++, getpid(), getppid(), getCurrentTime(), siginfo->si_pid);
	usleep(100000);
	kill(process5, SIGUSR1);
	usleep(100000);
	kill(process6, SIGUSR1);
	usleep(100000);
	kill(process7, SIGUSR1);
}

void Proc5(int signo, siginfo_t *siginfo, void *context)
{
	//usleep(100000);
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC4 PUT SIGUSR1\n", msg, getpid(), getppid(), getCurrentTime());
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC5 GET SIGUSR1 FROM %d\n", msg++, getpid(), getppid(), getCurrentTime(), siginfo->si_pid);
}

void Proc6(int signo, siginfo_t *siginfo, void *context)
{
	usleep(100000);
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC4 PUT SIGUSR1\n", msg, getpid(), getppid(), getCurrentTime());
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC6 GET SIGUSR1 FROM %d\n", msg++, getpid(), getppid(), getCurrentTime(), siginfo->si_pid);
}

void Proc7(int signo, siginfo_t *siginfo, void *context)
{
	//usleep(100000);
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC4 PUT SIGUSR1\n", msg, getpid(), getppid(), getCurrentTime());
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC7 GET SIGUSR1 FROM %d\n", msg++, getpid(), getppid(), getCurrentTime(), siginfo->si_pid);
	usleep(100000);
	kill(process8, SIGUSR1);
}

void Proc8(int signo, siginfo_t *siginfo, void *context)
{
	//usleep(100000);
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC7 PUT SIGUSR1\n", msg++, getpid(), getppid(), getCurrentTime());
	printf("%d.   PID = %d  PPID = %d    TIME = %i    PROC8 GET SIGUSR1 FROM %d\n", msg++, getpid(), getppid(), getCurrentTime(), siginfo->si_pid);
	usleep(10000);
	kill(process1, SIGUSR2);
}


void main()
{
	struct sigaction sa1, sa2, sa3, sa4, sa5, sa6, sa7, sa8;

	memset(&sa1, '\0', sizeof(sa1));
	sa1.sa_sigaction = &Proc1;
	sa1.sa_flags = SA_SIGINFO;
	
	memset(&sa2, '\0', sizeof(sa2));
	sa2.sa_sigaction = &Proc2;
	sa2.sa_flags = SA_SIGINFO;
	
	memset(&sa3, '\0', sizeof(sa3));
	sa3.sa_sigaction = &Proc3;
	sa3.sa_flags = SA_SIGINFO;

	memset(&sa4, '\0', sizeof(sa4));
	sa4.sa_sigaction = &Proc4;
	sa4.sa_flags = SA_SIGINFO;

	memset(&sa5, '\0', sizeof(sa5));
	sa5.sa_sigaction = &Proc5;
	sa5.sa_flags = SA_SIGINFO;

	memset(&sa6, '\0', sizeof(sa6));
	sa6.sa_sigaction = &Proc6;
	sa6.sa_flags = SA_SIGINFO;

	memset(&sa7, '\0', sizeof(sa7));
	sa7.sa_sigaction = &Proc7;
	sa7.sa_flags = SA_SIGINFO;

	memset(&sa8, '\0', sizeof(sa8));
	sa8.sa_sigaction = &Proc8;
	sa8.sa_flags = SA_SIGINFO;

	process1 = fork();
	if(process1 == 0)
	{
		process1 = getpid();
		sigaction(SIGUSR2, &sa1, NULL);

		process2 = fork();
		if(process2 == 0)
		{
			process2 = getpid();
			sigaction(SIGUSR1, &sa2, NULL);
			
			process4 = fork();
			if(process4 == 0)
			{
				process4 = getpid();
				sigaction(SIGUSR2, &sa4, NULL);
				while(1){}
			}
			
			process5 = fork();
			if(process5 == 0)
			{
				process5 = getpid();
				sigaction(SIGUSR1, &sa5, NULL);
				
				process6 = fork();
				if(process6 == 0)
				{
					process6 = getpid();
					sigaction(SIGUSR1, &sa6, NULL);
					
					process7 = fork();
					
					if(process7 == 0)
					{
						process7 = getpid();
						sigaction(SIGUSR1, &sa7, NULL);
						while(1){}
					}

					process8 = fork();
					if(process8 == 0)
					{
						process8 = getpid();
						sigaction(SIGUSR1, &sa8, NULL);
						while(1){}
					}

					while(1){}
				}

				while(1){}
			}
			while(1){}
		}
		process3 = fork();
		if(process3 == 0)
		{
			process3 = getpid();
			sigaction(SIGUSR1, &sa3, NULL);
			while(1){}
		}
		
		sleep(4);
		kill(process2, SIGUSR1);
		kill(process3, SIGUSR1);

		while(1){}
	}
}
