/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void process (int sock) { // do the reading, encrypting and writing
	int n;
	char plainBuffer[75000];
	char keyBuffer[75000];
	//char buffer[75000];
   
   //init variables and set to 0
	bzero(plainBuffer,75000);
	n = read(sock,plainBuffer,74999);
	if (n < 0) error("ERROR reading from socket");
	//printf("Here is the Plain: %s\n",plainBuffer);
	n = write(sock,"enc",3); //used to make sure dec wont be running enc_d
	n = read(sock,keyBuffer,74999);
	if (n < 0) error("ERROR reading from socket");
	//printf("Here is the Key: %s\n",keyBuffer);
	 
		
	//CHECK FOR BAD CHARACTERS
	if(plainBuffer[0] == '#'){ //set this in otp_enc
		printf("\n\nError, bad characters\n\n");
		exit(1);
	}
	 
	 
	 
	//ENCRYPTION
	char eBuffer[75000];
	int i = 0;
	int a, b, f;
	char c;
	do{
		a = (int) plainBuffer[i];
		b = (int) keyBuffer[i];
		if( a == 32){//if it's a space, set to 0, change back later
			a = 0;
		}
		else{
			a = (a-64);
		}
		if( b == 32){ //if it's a space
			b = 0;
		}
		else{
			b = (b-64);
		}
		f = ((a + b)%27); //26 letters + space
		if( f == 0){
			f = 32;
		}
		else{
			f = (f+64);
		}
		eBuffer[i] = (char) (f);
		i++;
		c = plainBuffer[i];
		}
	while((c != '\0' && c != '0' && c!= '\n') && i < 75000); //check for end of buffer
	eBuffer[i] = '\0';
	n = write(sock,eBuffer,75000);
	if (n < 0) error("ERROR writing to socket");
}

int main(int argc, char *argv[]) {
     
	 //Server code
	 
	int sockfd, newsockfd, portno;
	socklen_t clilen;
    //char buffer[75000];
    struct sockaddr_in serv_addr, cli_addr;
    //int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     
	 while(1){ //loop to keep the server running
		 
		 newsockfd = accept(sockfd, 
					 (struct sockaddr *) &cli_addr, 
					 &clilen);
		 if (newsockfd < 0) 
			  error("ERROR on accept");
		 
		 pid_t pid = fork(); //fork
		  
		if(pid < 0){ 
			printf("FAIL");
			_exit(2);
		}
		
		if (pid == 0) {
			/* This is the client process */
			close(sockfd);
			process(newsockfd); // do the encryption on child process
			exit(0);
		} else {
			close(newsockfd); // parent, go back to loop, be ready for another accept
		}
	 }
}
