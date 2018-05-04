#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "string.h"
#include <signal.h>
#include <math.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define BUF_SIZE 64
#define NUMBER_OF_SEMAPHORES 3

pid_t process1, process2;
int pipe1fd[2], pipe2fd[2]; 
int semaphore;
struct sigaction sa1;
struct sembuf semaphoreIncrement[1];
struct sembuf semaphoreDecrement[1];
char buf[BUF_SIZE];
int stringCount = 0;


int getCurrentTime(){

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int timeInMilliseconds = (int)(ts.tv_nsec / 1000000);
	return timeInMilliseconds;
	
}


/*void Proc1(int signo, siginfo_t *siginfo, void *context)
{
	char buf[BUF_SIZE];

	while(stringCount < 1000)
	{
		if (semop(semaphore, semaphoreDecrement, 1) == -1)
		{
			printf("Error decrementing semaphore\n");
		}

		for(int i = 0; i < 100; i++)
		{
			printf("%d %d %d\n", stringCount, getpid(), getCurrentTime());
			stringCount++;
			sprintf(buf, "Message number: %d Sender PID: %d Time: %d", stringCount, getpid(), getCurrentTime());
			write(pipe1fd[1], buf, BUF_SIZE);
			stringCount++;
			sprintf(buf, "Message number: %d Sender PID: %d Time: %d", stringCount, getpid(), getCurrentTime());
			write(pipe2fd[1], buf, BUF_SIZE);
		}
		
		if(stringCount != 1000)
		{
			if (semop(semaphore, semaphoreIncrement, 1) == -1)
			{
				printf("Error incrementing semaphore\n");
			}
		}
		else
		{
			semaphoreIncrement[0].sem_num = 0;
			if (semop(semaphore, semaphoreDecrement, 1) == -1)
			{
				printf("Error decrementing semaphore\n");
			}
		}
	}		
}*/


int main()
{
	
	semaphore = semget(IPC_PRIVATE, 3, 0666 | IPC_CREAT);

	/*memset(&sa1, '\0', sizeof(sa1));
	sa1.sa_sigaction = &Proc1;
	sa1.sa_flags = SA_SIGINFO;*/

	if (pipe(pipe1fd) == -1){
			printf("Error creating pipe\n");
			return 1;
	}

	if (pipe(pipe2fd) == -1){
			printf("Error creating pipe\n");
			return 1;
	}

	if (semaphore == -1){
		printf("Error creating semaphore\n");
		return -1;
	}	

	union semun 
	{
		int val;
		struct semid_ds *buf;
		unsigned short *array;
		struct seminfo *__buf;

	} arguments;
	arguments.val = 0;

	for (int i = 0; i < NUMBER_OF_SEMAPHORES; i++)
	{
		if (semctl(semaphore, i, SETVAL, arguments) == -1 )
		{
			printf("Error setting value to a semaphore\n");
			return -1;
		}
	}

	semaphoreIncrement[0].sem_op = 1;
	semaphoreIncrement[0].sem_flg = 0;

	semaphoreDecrement[0].sem_op = -1;
	semaphoreDecrement[0].sem_flg = 0;

	//lets start
	
	process1 = fork();
	if(process1 == 0)
	{
		//inside process 1

		sigaction(SIGUSR2, &sa1, NULL);
		semaphoreIncrement[0].sem_num = 2;
		semaphoreDecrement[0].sem_num = 1;

		while(1)
		{
			if (semop(semaphore, semaphoreDecrement, 1) == -1)
			{
				printf("Error decrementing semaphore\n");
			}

			for(int i = 0; i < 100; i++)
			{
				stringCount++;
				sprintf(buf, "Message number: %d Sender PID: %d Time: %d", stringCount, getpid(), getCurrentTime());
				write(pipe1fd[1], buf, BUF_SIZE);
				stringCount++;
				sprintf(buf, "Message number: %d Sender PID: %d Time: %d", stringCount, getpid(), getCurrentTime());
				write(pipe2fd[1], buf, BUF_SIZE);
			}
			
			if(stringCount != 600)
			{
				if (semop(semaphore, semaphoreIncrement, 1) == -1)
				{
					printf("Error incrementing semaphore\n");
				}
			}
			else
			{
				semaphoreIncrement[0].sem_num = 0;
				if (semop(semaphore, semaphoreIncrement, 1) == -1)
				{
					printf("Error decrementing semaphore\n");
				}
			}
		}
	}
	else if(process1 > 0)
		{
			//inside main_proc
			process2 = fork();
			if(process2 == 0)
			{
				//inside process2	
		
				sigaction(SIGUSR2, &sa1, NULL);
				semaphoreIncrement[0].sem_num = 1;
				semaphoreDecrement[0].sem_num = 2;

				while(1)
				{
					if (semop(semaphore, semaphoreDecrement, 1) == -1)
					{
						printf("Error decrementing semaphore\n");
					}
			
					for(int i = 0; i < 100; i++)
					{
						stringCount++;
						sprintf(buf, "Message number: %d Sender PID: %d Time: %d", stringCount, getpid(), getCurrentTime());
						write(pipe1fd[1], buf, BUF_SIZE);
						stringCount++;
						sprintf(buf, "Message number: %d Sender PID: %d Time: %d", stringCount, getpid(), getCurrentTime());
						write(pipe2fd[1], buf, BUF_SIZE);
					}
		
					if(stringCount != 1000)
					{
						if (semop(semaphore, semaphoreIncrement, 1) == -1)
						{
							printf("Error incrementing semaphore\n");
						}
					}
					else
					{
						semaphoreIncrement[0].sem_num = 0;
						if (semop(semaphore, semaphoreIncrement, 1) == -1)
						{
							printf("Error decrementing semaphore\n");
						}
					}
				}
			}
			else if(process2 > 0)
				{
					//inside main_proc

					semaphoreIncrement[0].sem_num = 1;
					semaphoreDecrement[0].sem_num = 0;

					/*kill(process1, SIGUSR2);
					usleep(100000);
					kill(process2, SIGUSR2);*/

					if (semop(semaphore, semaphoreIncrement, 1) == -1)
					{
							printf("Error incrementing semaphore \n");
							return -1;
					}

					if (semop(semaphore, semaphoreDecrement, 1) == -1)
					{
							printf("Error incrementing semaphore \n");
							return -1;
					}

					printf("output\n");
					int i = 1;
					while(i <= 1000)
					{
						usleep(10000);
						read(pipe1fd[0], buf, BUF_SIZE);
						printf("[%d]    From pipe1 : %s\n", i, buf);
						i++;
						read(pipe2fd[0], buf, BUF_SIZE);
						printf("[%d]    From pipe2 : %s\n", i, buf);
						i++;
					}

					printf("%d", stringCount);
	
					//while(1){}
				}
				else	
				{ 
					printf("Error creating process\n");
					return 1;
				}
		}
		else
		{
			printf("Error creating process\n");
			return 1;
		}	
}
