#include <stdio.h>  /* stdin, stdout */
#include <string.h>  /* memset(), strlen() */
#include <sys/ipc.h>  /* message queue */
#include <sys/msg.h>  /* message queue */
#include <sys/types.h>  /* message queue */
#include <unistd.h>  
#include <stdlib.h>  /* exit() */

#define BUF_SIZE 8000 /* buffer size */

struct msg {
	long mtype;  /* message type */
	char mtext[BUF_SIZE];  /* message data array */
    
};

static int total_size;  /* total text size */

/* Message Queue */

int main() {

	FILE* fp  /* file pointer */
	int msqid, file_size, ret_check;  /* message id / file size */
	key_t msq_key;  /* message queue key value */
	struct msg s_msg;
	char num, filename[1024];  /* num = 1: file or 2: keyboard / filename array */
	
	s_msg.mtype = 1;  /* message type */
	msq_key = ftok("msq",3333);  /* message queue key value */
	file_size = 0;  /* file size */
	total_size = 0;  /* total text size; */


	/* msgget */
	if ((msqid = msgget(msq_key, IPC_CREAT|0666)) < 0) { /* message identification number */
			
		  perror("msgget error")  /* msgget error */
			printf("\n");
			exit(1);
	}
	
	while(1) { 

		/* select input number */			
		printf("[1: File input, 2: Keyboard input] "); 
		scanf("%s", &num);  /* input number */

		/* case */
		switch(num) {
 
			case '1': {  /* num == 1 (input file) */
					
				printf("input file name: ");
				scanf("%s",filename);
					
				/* fopen */
				if ((fp = fopen(filename, "r")) != NULL) {  /* file open check */
							
					/* file size measuration */
					fseek(fp, 0, SEEK_END);
					file_size = ftell(fp);
					rewind(fp);

					memset(s_msg.mtext, 0, sizeof(s_msg.mtext));  /* memory initialization */		
					
					while(feof(fp) == 0) {  /* end of file check */ 

						/* fread */
						if ((ret_check = fread(s_msg.mtext, sizeof(char), 1000, fp)) == -1) {
											
							perror("Read Failed");
							printf("\n");
							exit(1);
						}
						
						/* To process B message send */
						if ((msgsnd(msqid,&s_msg, strlen(s_msg.mtext),0)) == -1) {
											
							perror("msgsnd failed");
							printf("\n");
							exit(1); 
						}

						/* msgsnd successful */
						else {
											
							printf("[Send Message]\n");
							printf("%s\n",s_msg.mtext);
											
							total_size = total_size + strlen(s_msg.mtext);
											
							printf("----------------------------\n");
							printf("Send Text Size: %ld\n", strlen(s_msg.mtext));
							printf("Total Text Size: %d\n",total_size);
							printf("File Size: %d\n",file_size);
							printf("----------------------------\n");
							
							memset(s_msg.mtext, 0, sizeof(s_msg.mtext));  /* memory initialization */
						}	
						 
					}  /* end while */

				}  /* end if */

				/* file open error */
				else { 
							
					printf("[File Open Error]");
					printf("\n");
					exit(1);
				}
					
				fclose(fp);  /* file close */
				break;  /* case '1' termination */

			} /* end case1 */

			case '2': {  /* num == 2 (input keyboard) */
						
				memset(s_msg.mtext, 0, sizeof(s_msg.mtext));  /* memory initialization */

				printf("[Input Message] ");
				scanf(" %[^\n]",s_msg.mtext);

			
					/* msgsnd */
					if ((msgsnd(msqid, &s_msg, strlen(s_msg.mtext), 0)) == -1) {
									
						perror("[Msgsnd Failed]");
						printf("\n");
						exit(1);
					}

					else { /*msgsnd successful */
									
						printf("[Send Message]\n");
						printf("%s\n",s_msg.mtext);
										
						total_size = total_size + strlen(s_msg.mtext);								     	 										
						printf("-----------------------------\n");
						printf("Send Text Size: %ld\n",strlen(s_msg.mtext));				
						printf("Total Text Size: %d\n",total_size);							     
						printf("-----------------------------\n");
								
						memset(s_msg.mtext, 0, sizeof(s_msg.mtext));								
				
					}

					break;  /* case '2' termination */

			} /* end case '2' */

			default: {
					
				printf("Check The Number\n");
				break; // default termination
			
			}  /* end default */
		
		}  /* end switch-case */

	}  /* end while */

	return 0;

}  /* end main */  




