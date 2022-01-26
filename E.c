#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>

#define SEM_NAME "/sem"
#define BUF_SIZE 8000

#define PORT 1111

typedef char* element;

typedef struct {
			
		int front, rear;
		element data[BUF_SIZE];

}QueueType;

void InitQueue(QueueType* queue); /* 큐 초기화 */
int IsFull(QueueType* queue);
int IsEmpty(QueueType* queue); /* 큐가 비어있는지 확인 */
void Enqueue(QueueType* queue, element item); /* 큐에 보관 */
element Dequeue(QueueType* queue); /* 큐에서 꺼냄 */

/* Inet UDP (processD -> processE) */

  int main() {
		
	/* UDP Variable */
	int server_sock, serv_addr_size;
 	struct sockaddr_in serv_addr;
	int port, option, ret_check, total_size, count;
  	char msg[BUF_SIZE];
	
	/* Shared Memory Variable */
	int shmid, semid;
    key_t shm_key, sem_key;
    char* shmSendBuf;
    char* tmp;  /* previous message address */
    char* message;
    QueueType queue;
 	sem_t* mysem;

	server_sock = socket(AF_INET, SOCK_DGRAM, 0); /* socket create (Inet UDP) */
	
	if (server_sock == -1) {
			
		perror("[Socket Error]");
		printf("\n");
		exit(1);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	
  	port = PORT;
  	option = 1;
  	setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&option, sizeof(option));

  	serv_addr.sin_family = AF_INET;
  	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  	serv_addr.sin_port = htons(port);

	ret_check = 0;

	ret_check = bind(server_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	if (ret_check == -1) {
			
		perror("[Bind Error]");
		printf("\n");
		exit(1);
	}

/* Shared Memory (processE -> processF) */

	sem_unlink(SEM_NAME);

	shm_key = ftok("shm_key",6666);
	total_size = 0;
	count = 0;
	shmid = shmget(shm_key, sizeof(key_t), IPC_CREAT| 0666);

	if (shmid == -1) {
		
		perror("[shmget error]");
		printf("\n");
		exit(1);
	}

	shmSendBuf = (char*)shmat(shmid, NULL, 0);

	if (shmSendBuf == (char*) -1) {
			
		perror("[shmat Error]");
		printf("\n");
		exit(1);
	}

	mysem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
		
	if (mysem == NULL) {
		
		perror("sem open error");
		exit(1);
	}

	
	while(1) {

			printf("[Waiting Message...]\n");
			
			InitQueue(&queue);
			serv_addr_size = sizeof(serv_addr);
			memset(msg, 0x00, sizeof(msg));
			
			/* recvfrom */
			ret_check = recvfrom(server_sock, msg, sizeof(msg), 0, 
							(struct sockaddr*)&serv_addr, &serv_addr_size);
			
			if (ret_check == -1) {
					
				perror("[recvfrom error]");
				printf("\n");
				break;
			}
		
			else { /* recvfrom success */

				printf("[Receive Message]\n");
				
				Enqueue(&queue, msg);

				message = (char*)malloc(sizeof(char)*BUF_SIZE);
				tmp = message;
				memset(message, 0, sizeof(message));

					
				sem_wait(mysem);  
				message = Dequeue(&queue);
	            printf("%s\n",message);	
				memcpy(shmSendBuf, message, strlen(message));
				sem_post(mysem);  
					
				total_size = total_size + strlen(message);
					
				printf("-----------------------------\n");
				printf("Receive Text Size: %ld\n", strlen(message));
				printf("Total Text Size: %d\n",total_size);
				printf("-----------------------------\n");

				message = tmp;
				free(message);
					
			}  /* end else */

	}  /* end while */

	close(server_sock);

	/* shmdt */
    ret_check = shmdt(shmSendBuf);
     
    if (ret_check == -1) {
         
        perror("Shmdt Error");
        exit(1);
    }
 
    /* shmctl */
    ret_check = shmctl(shmid, IPC_RMID, 0);
 
    if (ret_check == -1) {
             
        perror("Shmctl Error\n");
        exit(1);
    }

	sem_destroy(mysem);

	return 0;

}  /* end main */


void InitQueue(QueueType* queue) {
		
		queue->front = queue->rear = 0; /* front와 rear를 NULL로 설정 */
}

int IsEmpty(QueueType* queue) {
        
		return (queue->front == queue->rear);
}

int IsFull(QueueType* queue) {

		return ((queue->rear + 1) % BUF_SIZE == queue->front);

}

void Enqueue(QueueType* queue, element item) {
    
		if (IsFull(queue)) {
				perror("Queue is Full\n");
				exit(1);
		}			
		
		queue->rear = (queue->rear + 1) % BUF_SIZE;
		queue->data[queue->rear] = item;
		
}
				
element Dequeue(QueueType* queue) {

    if (IsEmpty(queue)) { /* 큐가 비어있는 경우 */
              
		fprintf(stderr,"Queue is Empty\n");
        exit(1);
    }

		queue->front = (queue->front + 1) % BUF_SIZE;
		return queue->data[queue->front];
}

		
