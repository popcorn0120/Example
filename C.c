#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/un.h>

#define TRUE 1
#define FALSE 0
#define BUF_SIZE 8000
#define PORT 2222
#define SERVER_PATH "unix"	

int main(int argc, char* argv[]) {


	/* TCP/IP variable */	
	int client_sock;
	struct sockaddr_in serv_addr, clnt_addr;
	char tcp_msg[BUF_SIZE];
	int port;  /* port number */

	/* Unix Domain UDP variable */
	int unix_server_sock;
	int option, total_size, ret_check;
	struct sockaddr_un unix_serv_addr;

/* TCP/IP (Process B -> Process C) */	
	
	/* socket create */				
	client_sock = socket(AF_INET, SOCK_STREAM, 0);
	/* socket(domain, type, protocol) */

	if (client_sock == -1) {
       		
		perror("[Socket Error]");
        printf("\n");
        exit(1);
    }

    port = PORT;  /* port number */

    memset(&serv_addr, 0, sizeof(serv_addr));  /* memory initialization */

    serv_addr.sin_family = AF_INET;  /* network type */
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    serv_addr.sin_port = htons(port);  /* port number */

/* Unix Domain UDP (Process C -> Process D) */
	
	if (access(SERVER_PATH, F_OK) == 0)  /* file exist check */
		unlink(SERVER_PATH);  /* file exist */
			/* unlink - remove [filename] file */
	
	/* socket create */
	unix_server_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	
	/* socket(domain, type, protocol) 
		 domain - AF_UNIX(Inner protocol), AF_INET(IPv4), AF_INET6(IPv6)
		 type - SOCK_STREAM(TCP), SOCK_DGRAM(UDP)
		 protocol - 0, IPPROTO_TCP(TCP), IPPROTO_UDP(UDP)
	*/

	if (unix_server_sock < 0) {
		
		perror("[Socket Error]");
		printf("\n");
		exit(1);
	}

	ret_check = 0;
	total_size = 0;

	memset(&unix_serv_addr, 0, sizeof(unix_serv_addr));

	unix_serv_addr.sun_family = AF_UNIX;  /* network type */
	strcpy(unix_serv_addr.sun_path, SERVER_PATH);  /* file system path */

	/* bind */
	ret_check = bind(unix_server_sock,(struct sockaddr*)&unix_serv_addr, sizeof(unix_serv_addr));

	if (ret_check == -1) {
		
		perror("[Bind Error]");
		printf("\n");
		exit(1);
	}

	/* connect */
	while(1) {
			
		ret_check = connect(client_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

		if (ret_check == -1) {
					
			printf("[Connecting Server...]\n");
			sleep(5);
		}

		else {
					
			printf("[Server Connection Complete!]\n");
			break;
		}
		
	}  /* end while */

	while(1) {
				
		memset(tcp_msg, 0, sizeof(tcp_msg));
			
		if (strlen(tcp_msg) == 0)
			printf("[Waiting Message...]\n");

		/* read */
		ret_check = read(client_sock,tcp_msg,sizeof(tcp_msg));
				
		if (ret_check == -1 || ret_check == 0) {
						
			printf("[Read Error]");
			printf("\n");
			break;
		}

		/* read successful */
		else {
						
			printf("[Receive Message]\n");
			printf("%s\n",tcp_msg);
						
			total_size = total_size + strlen(tcp_msg);
						
			printf("-----------------------------\n");
			printf("Receive Text Size: %ld\n", strlen(tcp_msg));
			printf("Total Text Size: %d\n",total_size);	
			printf("-----------------------------\n");					
					
			/* sendto */
			ret_check = sendto(unix_server_sock, tcp_msg, strlen(tcp_msg), 0, 
						(struct sockaddr*)&unix_serv_addr, sizeof(unix_serv_addr));
					
			if (ret_check == -1) {		
							
				perror("[Send Failed]");
				printf("\n");
				exit(1);
			}

				
		}  /* end else */
	

	}  /* end while */
	
	
	close(client_sock);  /* close socket */


}  /* end main */

