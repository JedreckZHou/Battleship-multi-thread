/****************************
* Author: Cristian Gustavo Castro
*  Univeristy of the Valley of Guatemala
*  Operating systems
*
*  Purpose: Main file from the client side.
*
*
*
***************************/


#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include <errno.h>

// define paquete genérico
struct package
{
    char *id;
    char *type;
	char *size;
	char *payload;
    char *checksum;
};

typedef enum {PLAYER1, PLAYER2} players;
players play = PLAYER1;

// Abre canal de comunicación entre servidor y cliente
int open_wendy(int fifo_server){

	fifo_server=open("/tmp/wendy",O_RDWR);
	if(fifo_server < 0) {
	  	printf("Error in opening wendy file \n");
		exit(-1);
	}

	return fifo_server;

}
// Crear pipe específico de comunicación
void create_input_pipe(char direccion[]){

		int unique_file = mkfifo(direccion,0666);
		if(unique_file<0) {
			 // If the file already exists, delete it
	 		printf("Unable to create a fifo \n");
	 		exit(-1);
	 	}

}
 // Abrir pipe específico de comunicación
int open_input_pipe(char direccion[], int fifo_client){

		fifo_client=open(direccion,O_RDONLY );

		if(fifo_client < 0) {
	  		printf("Error in opening input pipe file");
			exit(-1);
	    }

	 	return fifo_client;
}

// lee respuesta por parte del servidor

int read_response(char *buf, int fifo_client){

	 	buf=malloc(30*sizeof(char));


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

// Menú de opciones
void menu_game(){

	printf("\t ********** BIENVENIDO ********** \n");
	printf("\t MENU \n");
	printf("1. Crear juego \n");
	printf("2. Ver listado de juegos \n");
	printf("3. Salir \n");
	printf("Ingrese su opcion \n");

}


// Función que maneja el juego
void manage_game(int fifo_output, int choice, int fifo_input){

		char address_output[] = "/tmp/o_";
	 	char st_out[8];
	 	char *tmp;
	 	int opcion=0, respuesta = 0;
	 	int tx = 0;
	 	/** Establecer conexiÃ³n con el ouput pipe **/

	 	tmp=malloc(10*sizeof(char));
	 	sprintf(st_out, "%d", choice);
		strcat(address_output,st_out);	
		//free(tmp);		
  		int tmp_fifo = -1;
  		while(tmp_fifo < 0){

			fifo_output = open(address_output,O_WRONLY);		
			tmp_fifo = fifo_output;
		}
		
	//  while(tx!=3){
	  		
	  		char *buf_resp;
	  		buf_resp = malloc(30*sizeof(char));

		  	menu_game();
		  	scanf("%d",&tx);
		  	fflush( stdin );


			int err = write(fifo_output,&tx,sizeof(int));

			if(err < 0){
		 		printf("Failed to write in file \n");
		    }

		    fifo_input = read_response(buf_resp, fifo_input);
	//	}	

		printf ("Esperando rival... \n");
	  	

	  	char *buf_alloc;
		buf_alloc=malloc(20*sizeof(char));	
 		read (fifo_input,buf_alloc,20*sizeof(char));	  
 		printf("%s \n", buf_alloc);


 		printf ("Esperando turno... \n");
	  	int cl = 0;
	  	char *buf;
		buf=malloc(30*sizeof(char));	
 		int r = read (fifo_input,buf,20*sizeof(char));	  
 		if(buf == "Tu turno"){
 			printf("%s \n", buf);
 		}

		while(cl!=5){
		  		char *buf_r;
		  		buf_r = malloc(30*sizeof(char));

			  	printf("Ingrese numero para enviar \n");
			  	scanf("%d",&cl);

				int err = write(fifo_output,&cl,sizeof(int));

				if(err < 0){
			 		printf("Failed to write in file \n");
			    }
			    printf("Esperando resuesta... \n");
			    fifo_input = read_response(buf_r, fifo_input);
			    
		}

	close(fifo_output);

}

void main()
{
	 	int fifo_server,fifo_client, fifo;
	 	char str[256];
	 	char *buf;
	 	int choice=1;
	 	
	 	
	 	/*
                *********************handshake**********************
	 	*/
	 	// Get PID client
	 	printf("Estableciendo conexion... \n");	 	
	 	choice = getpid();

	 	// Establecer conexiÃ³n con wendy . Fifo_server es wendy
	 	fifo_server = open_wendy(fifo_server);


		// CreaciÃ³n de pipe de comunicaciÃ³n i_PID
	 	char direccion[] = "/tmp/i_";
	 	char st[8];
	 	sprintf(st, "%d", choice);
		strcat(direccion,st);
		create_input_pipe(direccion);

		// Enviar PID por Wendy
	 	write(fifo_server,&choice,sizeof(int));	 	


	 	// Abrir input_{PID}
	 	fifo_client  = open_input_pipe(direccion,fifo_client);

	 	// Leer respuesta del servidor
	 	fifo_client = read_response(buf, fifo_client);
		

	 	/*
                *********************Fin del handshake**********************
	 	*/
	 	/*
                *********************Inicio juego**********************
	 	*/

	 	manage_game(fifo, choice, fifo_client);

	 	/*
                *********************Inicio del handshake**********************
	 	*/

	 	close(fifo_server);
	 	close(fifo_client);
}