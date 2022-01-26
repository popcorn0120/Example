#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h>
#include <sys/sem.h>
#include "log_util.h"
#include <semaphore.h>
#include <fcntl.h>

#define SEM_NAME "/sem"
#define BUF_SIZE 8000

/* Shared Memory (processE -> processF) */


int main(int argc, char** argv) {

	OpenConfig(argv[0]);  /* user-defined function */

	/* Shared Memory Variable */
	int shmid, ret_check, total_size, semid, count;
	key_t shm_key, sem_key;
	char* shm_buf;
	
	char shm_msg[BUF_SIZE];
	FILE* fp;
	sem_t* mysem;
	
	sem_unlink(SEM_NAME);
	shm_key = ftok("shm_key",6666);
	sem_key = ftok("sem",3333);
	total_size = 0;
	ret_check = 0;	
	count = 0;

	//LOG_ERROR("ERROR TEST");
	
	shmid = shmget(shm_key, sizeof(key_t), IPC_CREAT|0666);

	if (shmid == -1) {
		
		LOG_ERROR("[Shmget Error]");
		exit(1);
	}
	

	shm_buf = (char*)shmat(shmid, NULL, 0);
	
	if (shm_buf == (char*) -1) {
		
		LOG_ERROR("[Shmat Error]");
		exit(1);
	}

  	fp = fopen("abc.txt", "ab");

	if (fp == NULL) {
		
		LOG_ERROR("File Open Error");
		exit(1);

	}

	mysem = sem_open(SEM_NAME, O_CREAT, 0666, 1);

	if (mysem == NULL) {
		
		LOG_ERROR("sem open error");
		exit(1);
	}
	
	while(1) {

		memset(shm_msg, 0, sizeof(shm_msg));

		if (strcmp(shm_buf,"") == 0) {
			
			if (count == 0) {
				printf("[Waiting Message...]\n");
				count++;
			}
		}
		
		else {  
							
			LOG_TRACE ("strcmp(shm_buf, "") != 0");			
			sem_wait(mysem);	
						
			printf("[Receive Message]\n");
			strncpy(shm_msg, shm_buf,strlen(shm_buf));
			printf("%s\n",shm_msg);
			LOG_INFO("%s\n",shm_msg);

			/* total size */
			total_size = total_size + strlen(shm_msg);
			
			printf("-----------------------------\n");
			printf("Receive Text Size: %ld\n",strlen(shm_msg));
			printf("Total Text size: %d\n",total_size);
			printf("-----------------------------\n");
			
			memset(shm_buf, 0, strlen(shm_buf));		
			sem_post(mysem);	
			count = 0;
					
			/* file write */
			ret_check = fwrite(shm_msg,sizeof(shm_msg),1,fp);
			
			if (ret_check == -1) {
					
				LOG_ERROR("[File Write Error]");
				printf("\n");
				break;
			}
		
			memset(shm_msg, 0x00, sizeof(shm_msg));

		}  /* end else */

	}  /* end while */

	/* shmdt */
	ret_check = shmdt(shm_buf);
	
	if (ret_check == -1) {
		
		LOG_ERROR("Shmdt Error");
		exit(1);
	}

	/* shmctl */
	ret_check = shmctl(shmid, IPC_RMID, 0);

	if (ret_check == -1) {
			
		LOG_ERROR("Shmctl Error\n");
		exit(1);
	}

	fclose(fp);
	sem_destroy(mysem);
	return 0;


}  /* end main */



						


