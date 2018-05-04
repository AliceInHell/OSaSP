#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "string.h"
#include <math.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h>

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


int main()
{
	
	int sharedMemory = shm_open("/sharedMemory", O_CREAT | O_RDWR, 0666);
	if (sharedMemory == -1)
	{
		printf("Error creating shared memory\n");
		return -1;
	}

	char *mapArea = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemory, 0);
	if (mapArea == MAP_FAILED)
	{
		printf("Error mmapping\n");
		return -1;
	}
	ftruncate(sharedMemory, BUF_SIZE);

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
				printf("%s", mapArea);
				sprintf(mapArea, "[2] PID: %d PPID: %d Time: %d  Written by: process2\n", getpid(), getppid(), getCurrentTime());
	
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
							printf("%s", mapArea);
							sprintf(mapArea, "[4] PID: %d PPID: %d Time: %d  Written by: process4\n", getpid(), getppid(), getCurrentTime());
	
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
								printf("%s", mapArea);
								sprintf(mapArea, "[3] PID: %d PPID: %d Time: %d  Written by: process3\n", getpid(), getppid(), getCurrentTime());
	
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
							printf("%s", mapArea);
							sprintf(mapArea, "[1] PID: %d PPID: %d Time: %d  Written by: process1	\n", getpid(), getppid(), getCurrentTime());
	
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

			sprintf(mapArea, "Lets start!\n");
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
