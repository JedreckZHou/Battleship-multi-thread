#include <stdio.h> 
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>    /* POSIX Threads */
#include <stdbool.h>


typedef struct str_thdata
{
    int thread_no;
    int id; // PID
    int index;
    int input_pipe;
    char message[100];
} thdata;

typedef struct info_thread
{
	int busy;
	int live;
} inf_th;

 struct package
 {
   char *id;               // Id de quién lo envia.
   char *type;             // Tipo del mensaje.
   int size;               // Tamaño del payload.
   char *payload;          // El mensajes enviado.
 };

typedef struct game
 {
   char *id;               // Id del juego
   char *size;             // Tamaño del tablero
   char *name;             // Nombre del juego
  // char *state;            // Estado del juego
   int id_creator;       // Id del creador del juego.
   int id_guest;            // Id del oponente
   int active;
   pthread_cond_t      condA;
   pthread_cond_t      condB;
   pthread_mutex_t     mutex;
   enum { STATE_A, STATE_B } state;
 } inf_game;

// Definición de variables globales 
inf_game games[100]; 
int indice=0;
int juegos=0;
typedef enum {PLAYER1, PLAYER2} players; 


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



/* Función que controla el flujo de un cliente */

void player ( void *ptr )
{   

    thdata *data;            
    data = (thdata *) ptr;  /* type cast to a pointer to thdata */

    int fifo_output, fifo_input;
	char address[25] = "/tmp/o_";
	char address_input[25] = "/tmp/i_";
	char stfile[8];
	char stfile_i[8];
	char *b;
	char *respuesta;
	int pipe_id = data->id;
	int local_index = data->index;
	int numero_juego;
	bool invitado;
	players play;
	
			/**                OUTPUT PIPE                            **/			
			// Se crea la dirección de comunicación para el ouput pipe
	sprintf(stfile, "%d", pipe_id);
	strcat(address,stfile);
			// Se crea la dirección de comunicación para el input pipe	
	sprintf(stfile_i, "%d", pipe_id);
	strcat(address_input,stfile_i);

			// Se crea el output pipe
	int file = mkfifo(address,0666); 
	if(file<0) {
	 	printf("Unable to create a fifo \n ");
	 	exit(-1);
	}   
	fifo_output = open(address,O_RDONLY);
	if(fifo_output<1) {
	 	printf("Error opening input client file \n ");
	}

	fifo_input = data->input_pipe;	
	fifo_input =  open_input_pipe(fifo_input,address_input);


			/**                Comunicación con el cliente           **/
	int c;
	int rd = 0;
	//while(c!=3){

		// Leer entrada del cliente
		rd = read (fifo_output,&c,sizeof(int));
		printf("TMP: %d \n", c);

		// Escribir respuesta al cliente
		fifo_input =  open_input_pipe(fifo_input, address_input);
		write_response_confirmation(fifo_input, respuesta, rd);

		// Crear juego
		if(c==1){
			games[juegos].id_creator = pipe_id;
			play = PLAYER1;
			numero_juego = juegos;
			games[numero_juego].state = STATE_A;
			juegos++;
		}
		// Unirse a un juego
		if(c==2){
			int i=0;
			for(i=0;i<indice;i++){
				if(games[i].id_guest == 0){
					numero_juego = i;
					games[i].id_guest = pipe_id;
					games[i].state = STATE_A;
					games[i].active = 1;
					play = PLAYER2;
					close(fifo_input);
					break;
				}
			}

		}

		close(fifo_input);
//	}
	while(games[numero_juego].active != 1){

	}

	fifo_input =  open_input_pipe(fifo_input, address_input);
	char *respuesta_inicio = "¡Inicia juego!";
	write(fifo_input,respuesta_inicio,15*sizeof(char));

	int jz = 0;

	while(jz != 5){
		if(play == PLAYER1){
			/* Esperar al estado A*/
		   	pthread_mutex_lock(&games[numero_juego].mutex);
		    while (games[numero_juego].state != STATE_A)
		        pthread_cond_wait(&games[numero_juego].condA, &games[numero_juego].mutex);
		    pthread_mutex_unlock(&games[numero_juego].mutex);

	 		
		    /* Interacción cliente a cliente */
	 		char *respuesta="Tu turno";
			write(fifo_input,respuesta,10*sizeof(char));
			
			rd = read (fifo_output,&jz,sizeof(int));
			printf("Respuesta del cliente 1: %d del juego %d \n", jz, numero_juego);

			/* Esperar al estado B*/
		    pthread_mutex_lock(&games[numero_juego].mutex);
	        games[numero_juego].state = STATE_B;
	        pthread_cond_signal(&games[numero_juego].condB);
	        pthread_mutex_unlock(&games[numero_juego].mutex);

		}
		else{
			/* Esperar al estado B*/
		   	pthread_mutex_lock(&games[numero_juego].mutex);
		    while (games[numero_juego].state != STATE_B)
		        pthread_cond_wait(&games[numero_juego].condB, &games[numero_juego].mutex);
		    pthread_mutex_unlock(&games[numero_juego].mutex);

	 		
		    /* Interacción cliente a cliente */
	 		respuesta="Tu turno";
			write(fifo_input,respuesta,10*sizeof(char));
			
			rd = read (fifo_output,&jz,sizeof(int));
			printf("Respuesta del cliente 2 : %d  del juego %d \n", jz, numero_juego);

			/* Esperar al estado A*/
		    pthread_mutex_lock(&games[numero_juego].mutex);
	        games[numero_juego].state = STATE_A;
	        pthread_cond_signal(&games[numero_juego].condA);
	        pthread_mutex_unlock(&games[numero_juego].mutex);

		}

	}	

	close(fifo_input);
	close(fifo_output);
	printf("Fin de comunicación \n");
   
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
	 		printf("Error opening input client file \n ");
	 	}
	 	return fifo_client;
}


void main()
{
	
	int fifo_server,fifo_client;

	
	pthread_t thread[100];
	//pthread_mutex_init(&lock, NULL);

	while (1){

		fifo_client=0;
		fifo_server=0;
		
		int id_client;
		char *buf;

	 	
	 	/*
                *********************handshake**********************
	 	*/

		// Abrir pipe de comunicación (Wendy)
		fifo_server = open_wendy(fifo_server);
		printf("\nAbriendo pipe de comunicación \n");
		read(fifo_server,&id_client,sizeof(int));

		// Abrir pipe de comunicación con el cliente (PID)
	 	char direccion[] = "/tmp/i_";
	 	char st[8];
	 	sprintf(st, "%d", id_client);
		strcat(direccion,st);
		fifo_client = open_input_pipe(fifo_client,direccion);
	 
	 	// Enviar información por tmp/i_{P  ID}
	 	// printf("Enviando informacion por pipe: %s \n", direccion);
	 	buf="hola";
	 	write(fifo_client,buf,20*sizeof(char)); 

		// printf("Cerrando pipes... \n");	
		// Cerrar comunicación con pipes
		close(fifo_server);
		close(fifo_client);

	 	/*
                *********************Fin del handshake**********************
	 	*/	 	
	
	 	thdata data;
	 	data.id = id_client;	 
	 	data.index = indice;
	 	data.input_pipe =  fifo_client; //open_input_pipe(fifo_client,direccion);

		
		
		pthread_create (&thread[indice], NULL, (void *) &player, (void *) &data);

		indice++;

		// printf("Sesión finalizada \n");

	}
}

