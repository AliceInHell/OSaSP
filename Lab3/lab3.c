#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

int getCurrentTime()
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int timeInMilliseconds = (int)(ts.tv_nsec / 1000000);
	return timeInMilliseconds;
}

int main()
{
	//Current time
	time_t t;
	struct tm *local;
	t = time(NULL);
	local = localtime(&t);
	
	//Processes
	pid_t pid;
	pid = fork();
	pid = fork();
	pid = fork();

	//Delay
	char delay;
	delay = (char)(getpid() * 0.002);

	for(char i = 0; i < 10; i++)
	{
		sleep(delay);
		printf("PID = %d   PPID = %d            TIME = %dmc  DATE = %s", getpid(), getppid(), getCurrentTime(), asctime(local));	
	}
	exit(0);			
}
