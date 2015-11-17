/* Mensaje de confirmación al servidor de recepción de mensaje */

void write_response_confirmation(int fifo_input, char *respuesta, int rd){
	if(rd  >= 0 ){
 		respuesta="Mensaje recibido correctamente ";
		write(fifo_input,respuesta,30*sizeof(char));
			
	}
	else{
		respuesta="Problemas en la recepcion del mensaje";
		write(fifo_input,respuesta,30*sizeof(char));
	}
}

/* Crea y abre el ouput_{PID}  */
int manage_output_pipe(char address[25], int fifo_output){
	int file = mkfifo(address,0666); 
	if(file<0) {
		
	 	printf("Error creating output client file \n ");
	 	
	}   

	fifo_output = open(address,O_RDONLY);
	if(fifo_output<1) {
	 	printf("Error opening output client file \n ");

	}
	return fifo_output;	
}

/* FUNCION PARA ABRIR EL PIPE WENDY */
int open_wendy(int fifo_server){
		fifo_server = open("/tmp/wendy",O_RDWR);
		if(fifo_server<1){	
	 		printf("Error opening file \n ");
	 	}
	 	return fifo_server;
}

/* Apertura del pipe i_{} */
int open_input_pipe(int fifo_client, char direccion[]){
		fifo_client = open(direccion,O_WRONLY);

		if(fifo_client<1) {
			printf ("Direccion: %s", direccion);
			perror(NULL);
	 		
	 	}
	 	return fifo_client;
}