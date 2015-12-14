#include <stdio.h>   
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

	//char buf[BUFSIZ];
	//fgets(buf, sizeof buf, stdin);

int main(int argc, char *argv[]){
	srand((int)time(NULL));
	int i;
	int num = atoi( argv[1] );
	char *key = (char *)malloc((num+2) * sizeof(char));
		for(i = 0; i < (num+1); i++){
		key[i] = 'A' + (random() % 26);
	}
	key[num+1]= '\0';
		int random = (rand() % ((num/10)+1));
	for(i=0; i < random; i++){
		key[(rand() % (num))] = ' ';
	}
	printf("%s", key);
    return 0;
}