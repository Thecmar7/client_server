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
	char cipherBuffer[75000];
	char keyBuffer[75000];
	//char buffer[75000];
   
	//init variables and set to 0
	bzero(cipherBuffer,75000);
	n = read(sock,cipherBuffer,74999);
	if (n < 0) error("ERROR reading from socket");
	//printf("Here is the Cipher: %s\n",cipherBuffer);
	n = write(sock,"dec",3); //used to make sure enc wont be running dec_d
	n = read(sock,keyBuffer,74999);
	if (n < 0) error("ERROR reading from socket");
	//printf("Here is the Key: %s\n",keyBuffer);
	
	//DECRYPTION
	 char eBuffer[75000];
	 int i = 0;
	 int a, b, f;
	 char c;
	 do{
		a = (int) cipherBuffer[i];
		b = (int) keyBuffer[i];
		if( a == 32){ //if it's a space, set to 0, change back later
			a = 0;
		}
		else{
			a = (a-64);
		}
		if( b == 32){//if it's a space
			b = 0;
		}
		else{
			b = (b-64);
		}
		f = ((a - b));
		if(f < 0){//in the event that b is larger than a, this will result in a negative so just add 27. it won't matter later because of the mod 27
			f += 27;
		}
		f = (f%27); //26 letters + space
		if( f == 0){
			f = 32;
		}
		else{
			f = (f+64);
		}
		eBuffer[i] = (char) (f);
		i++;
		c = cipherBuffer[i];
	 }
	 while((c != '\0' && c != '0' && c!= '\n') && i < 75000);//check for end of buffer
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
			process(newsockfd); // do the decryption on child process
			exit(0);
		}
		else { // parent, go back to loop, be ready for another accept
			close(newsockfd);
		}
		
	 }
}