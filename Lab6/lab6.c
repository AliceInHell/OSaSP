#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <wait.h>
#include <pthread.h>
#include <stdlib.h>
#include "dirent.h"
#include "string.h"

#define bool char
#define true 1
#define false 0
#define BUF_SIZE 200
#define MAX_FILES 400
#define FILE_NAME_SIZE 100

typedef struct arguments {
    char filePath1[FILE_NAME_SIZE];
    char filePath2[FILE_NAME_SIZE];
} arguments;

int runningThreads = 0;


struct FilePath
{
	char Path[FILE_NAME_SIZE];
	struct FilePath *Next;
} *Head1, *Head2;


struct FilePath* setMemory()
{
	return (struct FilePath*)malloc(sizeof(struct FilePath));
}


void setPath(int* count, DIR* dir, char* path, struct FilePath **p)
{
	struct dirent *fdFileInfo;
	while ((fdFileInfo = readdir(dir)) != NULL){
        if (fdFileInfo->d_type == DT_REG){

            strcpy((*p)->Path, path);
            strcat((*p)->Path, "/");
            strcat((*p)->Path, fdFileInfo->d_name);
			(*p)->Next = setMemory();
			(*p) = (*p)->Next;
			(*p)->Next = NULL;
            (*count)++;

        }
		else
		if ((fdFileInfo->d_type == DT_DIR) && (strcmp(fdFileInfo->d_name, ".")) && (strcmp(fdFileInfo->d_name, ".."))){
			char temp[100];
			strcpy(temp, path);
            strcat(temp, "/");
            strcat(temp, fdFileInfo->d_name);
			setPath(count, opendir(temp), temp, p);
		}
    }
}


/*void setPath(int* count, DIR* dir, char* path, char f[][400][100])
{
	struct dirent *fdFileInfo;
	while ((fdFileInfo = readdir(dir)) != NULL){
        if (fdFileInfo->d_type == DT_REG){

            strcpy((*f)[(*count)], path);
            strcat((*f)[(*count)], "/");
            strcat((*f)[(*count)], fdFileInfo->d_name);
            (*count)++;

        }
		else
		if ((fdFileInfo->d_type == DT_DIR) && (strcmp(fdFileInfo->d_name, ".")) && (strcmp(fdFileInfo->d_name, ".."))){
			char temp[100];
			strcpy(temp, path);
            strcat(temp, "/");
            strcat(temp, fdFileInfo->d_name);
			setPath(count, opendir(temp), temp, f);
		}
    }
}*/



void *compareFiles(void *args){

    arguments *structPointer = (arguments *) args;            

    int totalBytes = 0;
    bool equal = true;

    FILE *file1 = fopen(structPointer->filePath1, "r");
    FILE *file2 = fopen(structPointer->filePath2, "r");

    if (file1 == NULL || file2 == NULL){
	printf("Error opening file\n");
    }

    while (!(feof(file1) && (feof(file2)))){
        if (feof(file1) || feof(file2)){
            equal = false;
            break;
        }
        char c1 = fgetc(file1);
        char c2 = fgetc(file2);
        totalBytes += 1;
        if (c1 != c2){
            equal = false;
            break;
        }
    }

    printf("\nThread ID: %lu \nCOMPARING: %s %s \nBYTES COMPARED: %d RESULT: %s\n", pthread_self(), structPointer->filePath1, structPointer->filePath2, totalBytes, equal ? "EQUAL" : "NOT EQUAL");

    fclose(file1);
    fclose(file2);
    
    printf("\nThread %lu successfully terminated\n", pthread_self()); 
	//usleep(200000);   

    runningThreads--;

}



void error(char *msg){
    perror(msg);
    exit(1);
}



void main(int argc, char *argv[]){

    if (argc < 4){
        error("Enter correct number of arguments!");
    }


    char *dir1 = argv[1];
    char *dir2 = argv[2];
    int maxThreads = atoi(argv[3]);

	Head1 = setMemory();
	Head1->Next = setMemory();
	Head2 = setMemory();
	Head2->Next = setMemory();	
	struct FilePath *Head1Copy = Head1->Next;
	struct FilePath *Head2Copy = Head2->Next;

	
    /*char files1[MAX_FILES][FILE_NAME_SIZE];
    char files2[MAX_FILES][FILE_NAME_SIZE];*/


    DIR *firstDir = opendir(dir1);
    DIR *secondDir = opendir(dir2);

    if ((firstDir == NULL) || (secondDir == NULL)){
        error("Can't open directory");
    }

    struct dirent *fdFileInfo, *sdFileInfo;

    int filesCount1 = 0;
	int filesCount2 = 0;

	setPath(&filesCount1, firstDir, dir1, &Head1Copy);
	setPath(&filesCount2, secondDir, dir2, &Head2Copy);

	int filesSum = filesCount1 * filesCount2;

	arguments args[filesSum];
	int countPointer = 0;


	for (struct FilePath *temp1 = Head1->Next; temp1->Next != NULL; temp1 = temp1->Next)
	{
		for(struct FilePath *temp2 = Head2->Next; temp2->Next != NULL; temp2 = temp2->Next)
		{
			strcpy(args[countPointer].filePath1, temp1->Path);
	        strcpy(args[countPointer].filePath2, temp2->Path);

            while (runningThreads >= maxThreads){
                // wait until one of the threads ends
            }

	   		runningThreads++;
	        pthread_t newThread;

            if ( (pthread_create(&newThread, NULL, &compareFiles, (void *) &args[countPointer])) ){
                printf("Error creating thread");
                return;
            }

	    	countPointer++;
		}
	}		


    /*for (int i = 0; i < filesCount1; i++){
        for (int j = 0; j < filesCount2; j++){

	        strcpy(args[countPointer].filePath1, files1[i]);
	        strcpy(args[countPointer].filePath2, files2[j]);

            while (runningThreads >= maxThreads){
                // wait until one of the threads ends
            }

	   		runningThreads++;
	        pthread_t newThread;

            if ( (pthread_create(&newThread, NULL, &compareFiles, (void *) &args[countPointer])) ){
                printf("Error creating thread");
                return;
            }

	    	countPointer++;
        }
    }*/

	while (runningThreads != 0){
		usleep(10000);
	}


}
