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
#define NUMBER_OF_SEMAPHORES 4

pid_t process1, process2, process3, process4; 
int semaphore;
struct sembuf semaphoreIncrement[1];
struct sembuf semaphoreDecrement[1];
char buf[BUF_SIZE];


int getCurrentTime(){

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	int timeInMilliseconds = (int)(ts.tv_nsec / 1000000);
	return timeInMilliseconds;
	
}


int main(int argc, char* argv[])
{

	if (argc < 2){
		printf("Enter log file path\n");
		return -1;
	}


	//set vars
	
	char *filePath = argv[1];
	FILE* writefile = fopen(filePath, "w");
	FILE* readfile= fopen(filePath, "r");

	semaphore = semget(IPC_PRIVATE, 4, 0666 | IPC_CREAT);

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


	process1 = fork();
	if(process1 == 0)	
	{
		//inside process1

		semaphoreIncrement[0].sem_num = 1;
		semaphoreDecrement[0].sem_num = 0;

		process2 = fork();
		if(process2 == 0)
		{
			//inside process2

			semaphoreIncrement[0].sem_num = 2;
			semaphoreDecrement[0].sem_num = 1;

			while(1)
			{
				if (semop(semaphore, semaphoreDecrement, 1) == -1)
				{
					printf("Error incrementing semaphore\n");
					return -1;
				}

				//work

				printf("From file    [Process2]\n");
				for(int i = 0; i < 1; i++)
				{
					fgets(buf, BUF_SIZE, readfile);
					printf("%s\n", buf);
				}
	
				for(int i = 0; i < 2; i++)
				{
					sprintf(buf, "[2] PID: %d PPID: %d Time: %d  Written by: process2\n", getpid(), getppid(), getCurrentTime());
					fputs(buf, writefile);
				}
				
				fflush(writefile);
				usleep(300000);

				if (semop(semaphore, semaphoreIncrement, 1) == -1)
				{
					printf("Error incrementing semaphore\n");
					return -1;
				}
			}
		}
		else if(process2 > 0 )
			{
				//inside process1
				process3 = fork();
				if(process3 == 0)
				{
					//inside process3

					semaphoreIncrement[0].sem_num = 3;
					semaphoreDecrement[0].sem_num = 2;

					process4 = fork();
					if(process4 == 0)
					{
						//inside process4

						semaphoreIncrement[0].sem_num = 0;
						semaphoreDecrement[0].sem_num = 3;

						while(1)
						{
							if (semop(semaphore, semaphoreDecrement, 1) == -1)
							{
								printf("Error incrementing semaphore\n");
								return -1;
							}

							//work
							printf("From file    [Process4]\n");
							for(int i = 0; i < 3; i++)
							{
								fgets(buf, BUF_SIZE, readfile);
								printf("%s\n", buf);
							}

							for(int i = 0; i < 4; i++)
							{
								sprintf(buf, "[4] PID: %d PPID: %d Time: %d  Written by: process4\n", getpid(), getppid(), getCurrentTime());
								fputs(buf, writefile);
							}

							fflush(writefile);
							usleep(300000);

							if (semop(semaphore, semaphoreIncrement, 1) == -1)
							{
								printf("Error incrementing semaphore\n");
								return -1;
							}
						}
					}
					else if(process4 > 0)
						{
							//inside process3

							while(1)
							{
								if (semop(semaphore, semaphoreDecrement, 1) == -1)
								{
									printf("Error incrementing semaphore\n");
									return -1;
								}
	
								//work
								printf("From file    [Process3]\n");
								for(int i = 0; i < 2; i++)
								{
									fgets(buf, BUF_SIZE, readfile);
									printf("%s\n", buf);
								}
	
								for(int i = 0; i < 3; i++)
								{
									sprintf(buf, "[3] PID: %d PPID: %d Time: %d  Written by: process3\n", getpid(), getppid(), getCurrentTime());
									fputs(buf, writefile);
								}
	
								fflush(writefile);
								usleep(300000);
		
								if (semop(semaphore, semaphoreIncrement, 1) == -1)
								{
									printf("Error incrementing semaphore\n");
									return -1;
								}
							}
						}
						else
						{
							printf("Error creating a process\n");
							return -1;
						}
				}
				else if(process3 > 0)
					{
						//inside process1

						while(1)
						{
							if (semop(semaphore, semaphoreDecrement, 1) == -1)
							{
								printf("Error incrementing semaphore\n");
								return -1;
							}

							//work

							printf("From file    [Process1]\n");
							for(int i = 0; i < 4; i++)
							{
								fgets(buf, BUF_SIZE, readfile);
								printf("%s\n", buf);
							}
	
							for(int i = 0; i < 1; i++)
							{
								sprintf(buf, "[1] PID: %d PPID: %d Time: %d  Written by: process1\n", getpid(), getppid(), getCurrentTime());
								fputs(buf, writefile);
							}
	
							fflush(writefile);
							usleep(300000);

							if (semop(semaphore, semaphoreIncrement, 1) == -1)
							{
								printf("Error incrementing semaphore\n");
								return -1;
							}
						}
					}
					else
					{
						printf("Error creating a process\n");
						return -1;
					}
			}
			else
			{
				printf("Error creating a process\n");
				return -1;
			}
	}
	else if(process1 > 0)
		{
			//inside main
		
			semaphoreIncrement[0].sem_num = 0;

			if (semop(semaphore, semaphoreIncrement, 1) == -1)
			{
				printf("Error incrementing semaphore\n");
				return -1;
			}

			while(1)
			{
				//staying...
			}
		}
		else
		{
			printf("Error creating a process\n");
			return -1;
		}
}
