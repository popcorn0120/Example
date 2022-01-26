#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ipc.h>

#define BUF_SIZE 8000
#define PORT 1111
#define SERVER_PATH "unix"

int main() {

	/* Unix Domain UDP Variable */
	int unix_client_sock, unix_serv_addr_size;
	int ret_check, total_size;
	struct sockaddr_un unix_serv_addr;
	char unix_msg[BUF_SIZE];

	/* Inet UDP variable */
     int udp_server_sock;
     int port, option;
     struct sockaddr_in udp_serv_addr;
     socklen_t udp_serv_addr_size;

/* Unix Domain UDP (Process C -> Process D) */

	if (access(SERVER_PATH, F_OK) == 0)  /* file exist check */
			unlink(SERVER_PATH);  /* remove SERVER_PATH file */
	
	/* socket create */
	unix_client_sock = socket(AF_UNIX, SOCK_DGRAM, 0); /* Unix Domain UDP socket */

	if (unix_client_sock == -1) {
			
		perror("[Socket Error]");
		printf("\n");
		exit(1);
	}
	
	memset(&unix_serv_addr, 0, sizeof(unix_serv_addr));  /* memory initialization */
		
	unix_serv_addr.sun_family = AF_UNIX;  /* network type - AF_UNIX */
	strcpy(unix_serv_addr.sun_path, SERVER_PATH);  /* File System Path - SERVER_PATH */

	ret_check = 0;
	total_size = 0;
	
	/* bind */
	ret_check = bind(unix_client_sock, (struct sockaddr*)&unix_serv_addr,sizeof(unix_serv_addr));
		
	if (ret_check == -1) {	
			
		perror("[Bind Error]");
		printf("\n");
		exit(1);
	}	

/* Inet UDP (Process D -> Process E) */

	udp_server_sock = socket(AF_INET, SOCK_DGRAM, 0);  /* Inet UDP socket */
	
	if (udp_server_sock == -1) {
		
		perror("[Socket Error]");
		printf("\n");
		exit(1);
	}
	
	memset(&udp_serv_addr, 0, sizeof(udp_serv_addr)); /* memory initialization */

	port = PORT;  /* port number */
	option = 1; 

	/* port number duplicate */
	setsockopt(udp_server_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&option,sizeof(option));

	udp_serv_addr.sin_family = AF_INET;  /* network type - AF_INET(IPv4) */
	udp_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	udp_serv_addr.sin_port = htons(port);	 /* port number */

	/* bind */
	ret_check = bind(udp_server_sock, (struct sockaddr*)&udp_serv_addr, sizeof(udp_serv_addr));

	if (ret_check == -1) {
			
		perror("[Bind Error]");
		printf("\n");
		exit(1);
	}

	while(1) {

			memset(unix_msg, 0, sizeof(unix_msg));
			udp_serv_addr_size = sizeof(udp_serv_addr);			

			/* waiting message */
			if (strlen(unix_msg) == 0)
					printf("[Waiting Message...]\n");
			
			/* recvfrom */
			ret_check = recvfrom(unix_client_sock, unix_msg, sizeof(unix_msg), 0, 
						(struct sockaddr*) &unix_serv_addr, &unix_serv_addr_size);
				
			if (ret_check == -1) {
				
				perror("[Receive Failed]");
				printf("\n");
				break;
			}
			
			else {  /* recvfrom successful */
					
				printf("[Receive Message]\n");
				printf("%s\n",unix_msg);
					
				total_size = total_size + ret_check;

				printf("-----------------------------\n");
				printf("Receive Text Size: %d\n", ret_check);
		 		printf("Total Text Size: %d\n",total_size);
				printf("-----------------------------\n");

				/* sendto */
				ret_check = sendto(udp_server_sock, unix_msg, strlen(unix_msg), 0, 
						(struct sockaddr*)&udp_serv_addr, sizeof(udp_serv_addr));

				if (ret_check == -1) {

					perror("[Send Failed]");
					printf("\n");
					break;
				}

			
			}  /* end else */	
			
	}  /* end while */
			
		close(udp_server_sock);  /* close socket */
		close(unix_client_sock);  /* close socket */  
		
}  /* end main */
