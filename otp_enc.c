#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

	//char buf[BUFSIZ];
	//fgets(buf, sizeof buf, stdin);
	
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]){
	if (argc < 4) { //check for correct num of arguments
	   fprintf(stderr,"usage %s plaintext key port\n", argv[0]);
	   exit(0);
	}
	
	//check for files
	FILE *plainTextFile = fopen(argv[1], "r");
	FILE *keyFile = fopen(argv[2], "r");
	if (plainTextFile == 0 || keyFile == 0){
            printf( "Could not open file\n" );
	}
	else{
		//HANDLE PLAIN TEXT FILE, GET LENGTH
		fseek(plainTextFile, 0L, SEEK_END);
		int plainSize = ftell(plainTextFile);
		fseek(plainTextFile, 0L, SEEK_SET);
		//printf("%d\n", plainSize);
		
		char plainBuffer[75000];
		bzero(plainBuffer,75000);
		fgets(plainBuffer, 75000, plainTextFile);
		fclose(plainTextFile);
		
		//CHECK FOR BAD CHARACTERS
		int p, o;
		int errorBool = 0;
		for(p = 0; p < strlen(plainBuffer); p++){
			o = (int) plainBuffer[p];
			if ((o > 90) || (o < 65 && o > 32)){
				plainBuffer[0] = '#';
				errorBool = 1;
			}
		}

		
		
		//HANDLE KEY FILE, GET LENGTH
		fseek(keyFile, 0L, SEEK_END);
		int keySize = ftell(keyFile);
		fseek(keyFile, 0L, SEEK_SET);
		//printf("%d\n", keySize);
		
		char keyBuffer[75000];
		bzero(keyBuffer,75000);
		fgets(keyBuffer, 75000, keyFile);
		fclose(keyFile);
		
		//CATCH BUFFER INIT
		char catchBuffer[75000];
		bzero(catchBuffer,75000);

		
		//CHECK THAT KEY >= PLAIN TEXT FILE
		if(keySize < plainSize){
			printf("Error, key too small\n");
			exit(1);
		}
		else{
			
			//USE THE PORT LUKE
			int sockfd, portno, n;
			struct sockaddr_in serv_addr;
			struct hostent *server;
			
			//***************char buffer[75000];
			
			portno = atoi(argv[3]);
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (sockfd < 0){
				error("ERROR opening socket");
			}
			
			char host[] = "localhost";
			server = gethostbyname(host);
			
			if (server == NULL) {
				fprintf(stderr,"ERROR, no such host\n");
				exit(0);
			}
			bzero((char *) &serv_addr, sizeof(serv_addr));
				serv_addr.sin_family = AF_INET;
				bcopy((char *)server->h_addr, 
				(char *)&serv_addr.sin_addr.s_addr,
				server->h_length);
			serv_addr.sin_port = htons(portno);
			if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
				error("ERROR connecting");
			}

			//printf("PlainBuf: %s\n", plainBuffer);
			//printf("keyBuf: %s\n", keyBuffer);
			 
				n = write(sockfd,plainBuffer,strlen(plainBuffer)); //send the plain text
				if (n < 0) 
					 error("ERROR writing to socket");
				bzero(catchBuffer,75000);
				n = read(sockfd,catchBuffer,74999);
				if(catchBuffer[0] != 'e'){ //used to make sure that otp_enc won't run otp_dec_d
					printf("Error, Wrong Use\n");
					exit(1);
				}
				n = write(sockfd,keyBuffer,strlen(keyBuffer)); //send the key
				if (n < 0) 
					 error("ERROR writing to socket");
				bzero(catchBuffer,75000);
				n = read(sockfd,catchBuffer,74999); //read the cipher text
				if (n < 0) 
					 error("ERROR reading from socket");
				if(errorBool == 0){ //don't print if there were illegal characters
					printf("%s\n",catchBuffer);
				}
				close(sockfd);
				return 0;
			}
			
			
			
	
	}
}