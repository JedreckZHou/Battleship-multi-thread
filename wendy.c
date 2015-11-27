/****************************
* Author: Cristian Gustavo Castro
*
*  Create a file called wendy in the /tmp directory
*  for estabilishing communication between client and server
*
*
***************************/

#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
main()
{
	int file1,file2;
	int fd;
	char str[256];
	file1 = mkfifo("/tmp/wendy",0666); 
	if(file1<0) {
 		printf("Unable to create a fifo");
 		exit(-1);
 	}

	printf("Wendy! :)");
}
