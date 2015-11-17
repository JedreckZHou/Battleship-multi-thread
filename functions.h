int open_wendy(int fifo_server){
	fifo_server=open("/tmp/wendy",O_RDWR);
	if(fifo_server < 0) {
	  	printf("Error in opening wendy file \n");
		exit(-1);
	}

	return fifo_server;

}

void getAddress(int pid, char* direccion){
	 	
	 	//char direccion[] = "/tmp/i_";
	 	char st[32];
	 	sprintf(st, "%d", pid);
		strcat(direccion,st);

	 	//printf ("st %s - direccion %s", st, direccion);
		//strcat(direccion,st);
		//return (char *)direccion;
		//return st;
}

void create_input_pipe(char direccion[]){

		
		int unique_file = mkfifo(direccion,0666); 
		if(unique_file<0) {
			 // If the file already exists, delete it    		
	 		perror(NULL);
	 		exit(-1);
	 	}

}

int open_input_pipe(char direccion[], int fifo_client){
	
		fifo_client=open(direccion,O_RDONLY );	 

		if(fifo_client < 0) {
	  		printf("Error in opening input pipe file");
			exit(-1);
	    }

	 	return fifo_client;
}

int read_response(char *buf, int fifo_client){
	  	
	 	buf=malloc(30*sizeof(char));
	 

	// 	int flags = fcntl(fifo_client, F_GETFL, 0);
	//	fcntl(fifo_client, F_SETFL, flags | O_NONBLOCK);

	 	int r = read (fifo_client,buf,30*sizeof(char));

	 	if(r<0){
	 		perror(NULL);	 		
	 	}
	 	else{
	 		printf("Respuesta del servidor: %s \n", buf);
	 	}

	 	free(buf);

	 	return fifo_client;

}
