#include <stdio.h>  /* stdin, stdout */
#include <string.h>  /* memset */
#include <sys/types.h>  /* message queue */
#include <sys/ipc.h>  /* message queue */
#include <sys/msg.h>  /* message queue */
#include <sys/socket.h>  /* TCP/IP */
#include <arpa/inet.h>  /* TCP/IP */
#include <unistd.h>  /* TCP/IP */
#include <stdlib.h>  /* exit */

#define BUF_SIZE 8000 /* buffer size */
#define PORT 2222 /* port number */

struct msg {
	long mtype;  /* message type */
	char mtext[BUF_SIZE];  /* message data array */
};

int main() {

	/* message queue variable */

	int ret_check, count;
	int msqid;  /* message queue id */
	key_t msq_key; /* message queue key value */
	struct msg msq;

	/* TCP/IP (process B -> process C) */
	/* TCP/IP variable */
	int port, total_size, option;
	int server_sock, client_sock;
	struct sockaddr_in serv_addr, clnt_addr;
	socklen_t clnt_addr_size;

	msq_key = ftok("msq",3333);
	ret_check = 0; 
	count = 0;

/* message queue (process A -> process B) */	
	/* msgget */
	msqid = msgget(msq_key, IPC_CREAT|0666); /* message queue id */
  
	if (msqid == -1) { 	

		perror("[Msgget Failed]"); // msgget failed
		printf("\n");
      	exit(1); 
	}
  
	/* socket create */
	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	
	if (server_sock == -1) {
		
		perror("[Socket Error]");
		printf("\n");
		exit(1);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));  /* memory initialization */
	option = 1;
	port = PORT;
	/* port number duplicate use */
	setsockopt(server_sock,SOL_SOCKET, SO_REUSEADDR, (void*)&option, sizeof(option));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	ret_check = 0;

	/* bind */
	ret_check = bind(server_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
		
	if (ret_check == -1) {	
			
		perror("[Bind Error]");
		printf("\n");
		exit(1);
	}

	/* listen */
	ret_check = listen(server_sock,5);

	if (ret_check == -1) {
			
		perror("[Listen Error]");
		printf("\n");
		exit(1);
	}	

	
	while(1) {
		
		memset(msq.mtext, 0, sizeof(msq.mtext));  /* memory initialization */
			
		if (strlen(msq.mtext) == 0)
			printf("[Waiting Message ...]\n");
			
		/* msgrcv */	
		ret_check = msgrcv(msqid, &msq, sizeof(msq.mtext),0,0);
			
		if (ret_check == -1) {	

			perror("[Msgrcv Failed]");
			printf("\n");
			exit(1);
		}

		/* msgrcv successful */
		else {

			printf("[Receive Message] \n");
			printf("%s\n",msq.mtext);
				
			total_size = total_size + strlen(msq.mtext);
				
			printf("-----------------------------\n");
			printf("Receive Text Size: %ld\n", strlen(msq.mtext));
			printf("Total Text Size: %d\n",total_size);
			printf("-----------------------------\n");
			
			/* first accept */
			if (count == 0) {
			
				/* accept */			
				clnt_addr_size = sizeof(clnt_addr);
				client_sock = accept(server_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
				
				count++;
				
				if (client_sock == -1) {
						
					perror("[Accept Error]");
					printf("\n");
					break;
				}

			}

			/* write */
			ret_check = write(client_sock, msq.mtext, strlen(msq.mtext));
					
			if (ret_check == -1) {
							
				perror("[Send Failed]");
				printf("\n");
				break;
			}
				
			memset(msq.mtext, 0, sizeof(msq.mtext));

		}  /* end else */

	}  /* end while */
	
	close(client_sock);  /* close socket */

}  /* end main */



		

