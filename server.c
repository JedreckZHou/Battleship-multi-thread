#include <stdio.h> 
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>    /* POSIX Threads */
#include <stdbool.h>
#include "server-functions.h"
#include "communication-functions.h"
/*
						 Definición de estructuras del juego 
						 											*/

typedef struct position{
  int x;
  int y;
}Posicion;

typedef struct str_thdata
{
    int thread_no;
    int id; // PID
    int index;
    int input_pipe;
    char message[100];
} thdata;

typedef struct game
 {
   int id;
   char name[100];     
   int id_creator; 
   int id_guest;   
   int active;
   int t1;
   int t2;
   int t3;
   int dimension;
   Posicion posP1[200];
   Posicion posP2[200];
   Posicion ataqueP1;
   Posicion ataqueP2;
   pthread_cond_t      condA;
   pthread_cond_t      condB;
   pthread_mutex_t     mutex;
   enum { STATE_A, STATE_B } state;
 } inf_game;


inf_game games[100]; 
int indice=0;
int juegos=0;
pthread_mutex_t lock;
typedef enum {PLAYER1, PLAYER2} players; 


Posicion get_positions(char word[255]){

	Posicion pos;
	char *tok;
	tok = strtok(word , ","); 
		
	int cnt = 0;
    while (tok != NULL){
        cnt++;    
        if(cnt == 1){
        	pos.x = atoi(tok);
        	//printf("POS.x : %d \n", pos.x);
        }
        else if(cnt == 2){
        	pos.y = atoi(tok);
        	//printf("POS.y : %d \n", pos.y);
        }
        tok = strtok(NULL,",");
    }

    return pos;

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
	char *token;
	bool invitado;
	players play;
	package pack;
	
	// Output pipe	
	sprintf(stfile, "%d", pipe_id);
	strcat(address,stfile);
	
	// Input pipe	
	sprintf(stfile_i, "%d", pipe_id);
	strcat(address_input,stfile_i);

	fifo_output = manage_output_pipe(address, fifo_output);
	fifo_input =  data->input_pipe;	
	fifo_input =  open_input_pipe(fifo_input,address_input);
	printf("Estamos en un nuevo thread \n");

	/**                Comunicación con el cliente           **/
	
	int rd;
	int flag_out = 0;
	int start_game = 0;
	
	/* Opciones:
		FA: Crear un nuevo juego
		F6: Solicitar un juego
		F8: Unirse a un juego
	*/
	while(flag_out != 1){

		pack = read_package(pack, fifo_output);
		fifo_input =  open_input_pipe(fifo_input, address_input);

		
		if(strcmp(pack.type,"FA") == 0){ // Crear juego
		
			play = PLAYER1; // Usted es jugador 1

			pthread_mutex_lock(&lock);	            // Proteger la variable juegos
			
			games[juegos].id_creator = pipe_id;			
			numero_juego = juegos;
			games[numero_juego].state = STATE_A;
			juegos++;
			pthread_mutex_unlock(&lock);

			games[numero_juego].id = pipe_id;		// Fin de protección variable juegos

			token = strtok(pack.payload , ","); 
		
			int counter_tokens = 0;
            while (token != NULL){
            	counter_tokens++;
                if(counter_tokens == 1)
					games[numero_juego].dimension = atoi(token);
				else if(counter_tokens == 2)
					strcpy(games[numero_juego].name, token);
				else if(counter_tokens == 3)
					games[numero_juego].t1 = atoi(token);					
				else if(counter_tokens == 4)
					games[numero_juego].t2 = atoi(token);
				else if(counter_tokens == 5)
					games[numero_juego].t3 = atoi(token);

                
                token = strtok(NULL,",");
        	}

        	printf ("Juego creado con los siguientes datos: \nDim: %d, Nombre: %s, T1: %d, T2: %d, T3: %d \n", 
        		games[numero_juego].dimension, games[numero_juego].name, games[numero_juego].t1,
        		games[numero_juego].t2, games[numero_juego]. t3);

        	flag_out = 1;
        	start_game = 3;

		}

		
		else if( strcmp(pack.type,"F6") == 0){ // Solicitar juegos
			int index_games;
			char list_games[255];			
			char tmpID[32];
			sprintf(tmpID, "%d", games[numero_juego].id );

			
			for (index_games = 0; index_games < juegos ; index_games++){
				if(games[index_games].active != 1){
					char tmpPayload[255];
					sprintf(tmpPayload, "%d-%s-%dx%d\n",
						games[index_games].id, games[index_games].name, 
						games[index_games].dimension,games[index_games].dimension);
					strcat(list_games, tmpPayload);
				}
			}
			// Envía lista de juegos
			pack = build_package(pack, tmpID , "F5", "10", list_games);
			send_package(pack, fifo_input);

			start_game = 2;
		}

		
		else if( strcmp(pack.type,"F8") == 0 ){ // Unirse a un juego
			int i=0;
			for(i=0;i<indice;i++){
				if((games[i].id == atoi(pack.payload)) && (flag_out != 1)){
					numero_juego = i;
					games[i].id_guest = pipe_id;
					games[i].state = STATE_A;
					games[i].active = 1;
					play = PLAYER2;
					printf("Juego con id: %d asignado! \n", games[i].id);

					start_game = 1;
					flag_out = 1;
				}
			}

		}

		/* Opciones:
			1: Confirmación de asignación de juego
			0: Recepción de mensaje
		*/

		if(start_game == 0){

			pack = build_package(pack, "-" , "00", "10", "Mensaje recibido"); // Envía paquete de recepción
			send_package(pack, fifo_input);

		}
		else if(start_game == 1){
			
			char tmpId[32];
			sprintf(tmpId, "%d", games[numero_juego].id);
			char info[255];
			sprintf(info,"%s,%d,%d,%d,%d",  games[numero_juego].name, 
				games[numero_juego].dimension, games[numero_juego].t1, 
				games[numero_juego].t2, games[numero_juego].t3);

			pack = build_package(pack, tmpId , "F9", "10", info); // Envía paquete de asignación
			send_package(pack, fifo_input);
			
		}		
		else if(start_game == 3){
			char tmpId[32];
			sprintf(tmpId, "%d", games[numero_juego].id);
			char info[255];
			sprintf(info,"Se ha creado un juego exitosamente, con ID: %d",  games[numero_juego].id);

			pack = build_package(pack, "-" , "FB", "10", info); // Envía paquete de asignación
			send_package(pack, fifo_input);			
		}

		close(fifo_input);
	}


	// Esperando a que el juego este activo para continuar

	while(games[numero_juego].active != 1){
		// do nothing
	}


	// Envía mensaje de Inicio de juego
	fifo_input =  open_input_pipe(fifo_input, address_input);
	pack = build_package(pack, pack.payload, "01", "10", "¡Inicia juego!");
	send_package(pack, fifo_input);


	//Leyendo posiciones de los barcos

	pack = read_package(pack, fifo_output);
	
	char *token_positions;
	token_positions = strtok(pack.payload , ","); 
		
	int ctrtokens = 0;
    while (token_positions != NULL){

    	//Posicion tempPos = get_positions(token_positions);

    	if(play == PLAYER1){
	    	if ((ctrtokens % 2) == 0){
				games[numero_juego].posP1[ctrtokens].y =  atoi(token_positions);
				printf("PLAYER 1 VAL: %d \n",games[numero_juego].posP1[ctrtokens].y );
	    	}    	
	    	else{
	    		games[numero_juego].posP1[ctrtokens].x =  atoi(token_positions);
	    		printf("PLAYER 1 VAL: %d \n",games[numero_juego].posP1[ctrtokens].x );
	    	}
    	}
    	else{
	    	if ((ctrtokens % 2) == 0){
				games[numero_juego].posP2[ctrtokens].y =  atoi(token_positions);
				printf("PLAYER 2 VAL: %d \n",games[numero_juego].posP2[ctrtokens].y );
	    	}    	
	    	else{
	    		games[numero_juego].posP2[ctrtokens].x =  atoi(token_positions);
	    		printf("PLAYER 2 VAL: %d \n",games[numero_juego].posP2[ctrtokens].x );
	    	}
    	}
        ctrtokens++;    
        token_positions = strtok(NULL,",");
    }

	// Fin de posicionamiento de barcos
	


	int jz = 0;
	while(jz != 5){
		if(play == PLAYER1){
			/* Esperar al estado A*/
		   	pthread_mutex_lock(&games[numero_juego].mutex);
		    while (games[numero_juego].state != STATE_A)
		        pthread_cond_wait(&games[numero_juego].condA, &games[numero_juego].mutex);
		    pthread_mutex_unlock(&games[numero_juego].mutex);

	 		
			pack = build_package(pack, pack.payload, "03", "10", "Tu turno!");
			send_package(pack, fifo_input);

			
			//rd = read (fifo_output,&jz,sizeof(int));
			pack = read_package(pack, fifo_output);
			printf("Respuesta del cliente 1: %d del juego %s \n", jz, pack.payload);

			/* Liberar al estado B*/
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

	 		
			pack = build_package(pack, pack.payload, "03", "10", "Tu turno!");
			send_package(pack, fifo_input);

			
			//rd = read (fifo_output,&jz,sizeof(int));
			pack = read_package(pack, fifo_output);
			printf("Respuesta del cliente 2: %d del juego %s \n", jz, pack.payload);			

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


void main()
{
	
	int fifo_server,fifo_client;

	pthread_t thread[400];

	while (1){

		fifo_client=0;
		fifo_server=0;
		package packet;	
		int id_client;
		char *buf;

	 	
	 	/*
                *********************handshake**********************
	 	*/

		fifo_server = open_wendy(fifo_server);
		printf("\n Abriendo pipe de comunicación \n");
		packet =  read_package(packet, fifo_server);

		
	 	char direccion[] = "/tmp/i_";
	 	char st[32];
	 	sprintf(st, "%s%s", direccion,packet.payload);
		fifo_client = open_input_pipe(fifo_client,st);	 

	 	id_client = atoi(packet.payload);
		
		packet = build_package(packet, packet.payload, "F1", "10", "Conexion establecida");
		send_package(packet, fifo_client);

		close(fifo_server);
		close(fifo_client);

	 	/*
                *********************Fin del handshake**********************
	 	*/	 	
	
	 	thdata data;
	 	data.id = id_client;	 
	 	data.index = indice;
	 	data.input_pipe =  fifo_client; 
		
		pthread_create (&thread[indice], NULL, (void *) &player, (void *) &data);

		indice++;		

	}
}

