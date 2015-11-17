#include <stdio.h> 
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ncurses.h>
#include "communication-functions.h"
#include "functions.h"
#include "interfaz.h"

typedef struct datagame{
	int dimension;
	int t1;
	int t2;
	int t3;
	char nombre[100];
}DataGame;

DataGame juegoActual;

DataGame crear_juego(){	
	
	
	initscr();
  	clear();
    if (has_colors()) start_color();
	refresh();
    cbreak();
    echo();
    keypad(stdscr, TRUE);


	int dim;
	char nombre[100];
	int tipo1, tipo2, tipo3;
	DataGame dataGame;

	printw("Ingrese los datos del juego: \n");
	printw("Dimensiones del juego nxn: \n");
	scanw("%d",&dim);
	printw("Nombre del juego: \n");
	scanw("%s",nombre);	
	printw("Cantidad de barcos de tipo 1 (1x1):");
	scanw("%d",&tipo1);
	printw("Cantidad de barcos de tipo 1 (2x1):");
	scanw("%d",&tipo2);
	printw("Cantidad de barcos de tipo 1 (3x2):");
	scanw("%d",&tipo3);	

	dataGame.dimension = dim;
	dataGame.t1 = tipo1;
	dataGame.t2 = tipo2;
	dataGame.t3 = tipo3;
	strcpy(dataGame.nombre, nombre);
	endwin();

	return dataGame;
}

int operacion(int respuesta, package pack, int id, int fifo_output, int fifo_input){

	char tmp_id[25];
	sprintf(tmp_id, "%d", id);
	
	if(respuesta == 1){ // Decide crear un juego
		DataGame dg = crear_juego();
		juegoActual = dg;
		char dataToSend[255];
		sprintf(dataToSend,"%d,%s,%d,%d,%d",dg.dimension, dg.nombre, dg.t1, dg.t2, dg.t3);
		pack = build_package(pack, tmp_id , "FA", "10", dataToSend);
		send_package(pack, fifo_output);
		
		return 1;
	}

	else if(respuesta == 2){ // Decide ver la lista de juegos
		
		initscr();
  		clear();
    	if (has_colors()) start_color();
		refresh();
    	cbreak();
    	echo();
    	keypad(stdscr, TRUE);

			

		int id_juegos[50];
		char *token, *token_id, *tokenizer;
		char resp_salida[8];
		pack = build_package(pack, tmp_id , "F6", "0", "-");
		send_package(pack, fifo_output);
		
		printw("Lista de juegos \n");
		
		pack =  read_package(pack, fifo_input); // Lee lista de juegos
		
		int cnt_toks = 0;
 		token = strtok(pack.payload , "\n"); 
        while (token != NULL){
        	cnt_toks++;
        	if(cnt_toks == 1)
        		printw("\t ID  -  nombre  -  tamanio \n");
            printw("\t %s\n", token);
            token = strtok(NULL,"\n");
        }


        if(cnt_toks > 0){ // Si hay más de un juego es posible unirse a uno
	        printw("Desea unirse a un juego (s/n) ? \n");
	        scanw("%s",resp_salida);
	        if(strcmp(resp_salida,"s") == 0){ // El usuario decide unirse a un juego
	        	int numJuego;
	        	printw("Ingrese id de juego del listado: ");
	        	scanw("%d",&numJuego);        	
	        	char tmpID_game[32];
	        	sprintf(tmpID_game, "%d", numJuego);
				pack = build_package(pack, tmp_id, "F8", "10", tmpID_game);
				send_package(pack, fifo_output);
				endwin();
				return 1;
	        }	
	        else{ // El usuario decide volver al menú principal
	        	endwin();
	        	return 2;
	        }	
        } 
        else{ // No hay juegos para ver
        	printw ("No hay juegos disponibles \n");
        	endwin();
        	return 2;
        }
	}

}
void establecer_datos_juego(char cadena[255]){
	char *token;
	token = strtok(cadena , ","); 
	int counter_tokens = 0;
    while (token != NULL){
        counter_tokens++;
		if(counter_tokens == 1)
			strcpy(juegoActual.nombre, token);
        else if(counter_tokens == 2)
			juegoActual.dimension = atoi(token);		
		else if(counter_tokens == 3)
			juegoActual.t1 = atoi(token);					
		else if(counter_tokens == 4)
			juegoActual.t2 = atoi(token);
		else if(counter_tokens == 5)
			juegoActual.t3 = atoi(token);
    
        token = strtok(NULL,",");
    }
}
void imprime_datos_juego(){

		initscr();
	  	clear();
    	if (has_colors()) start_color();
		refresh();
    	cbreak();
    	echo();
    	keypad(stdscr, TRUE);

		printw("Usted ha ingresado al siguiente juego: \n");
		printw("Nombre: %s \n", juegoActual.nombre );
		printw("Tamaño: %d x %d \n", juegoActual.dimension, juegoActual.dimension);
		printw("Barcos tipo 1x1 (H/V): %d \n", juegoActual.t1);
		printw("Barcos tipo 2x1 (H/V): %d \n", juegoActual.t2);
		printw("Barcos tipo 3x1 (H/V): %d \n", juegoActual.t3);

		endwin();
}

void manage_game(int fifo_output, int choice, int fifo_input){

	char address_output[] = "/tmp/o_";
	char st_out[8];
	char *tmp;
	int opcion=0, respuesta = 0;
	int respuesta_menu = 0;

	package pack;

	/** Establecer conexión con el ouput pipe **/

	
	sprintf(st_out, "%d", choice);
	strcat(address_output,st_out);	
  	int tmp_fifo = -1;
  	while(tmp_fifo < 0){
		fifo_output = open(address_output,O_WRONLY);		
		tmp_fifo = fifo_output;
	}
	
	/* Ingreso al menu de opciones 
		1: Crear juego
		2: Ver listado de juegos (Opción de unirse a un juego)
	*/
	int salida = 0;	
    while(salida != 1){
	  		
	  	char *buf_resp;
	  	buf_resp = malloc(30*sizeof(char));

		//menu_game();
		//scanf("%d",&respuesta_menu);
		//fflush( stdin );
		respuesta_menu =  menu_principal();
		

		salida = operacion(respuesta_menu, pack, choice, fifo_output, fifo_input);		

		if(salida != 2){
			pack =  read_package(pack, fifo_input);
			
			if(strcmp(pack.type, "F9") == 0){ // Confirmación de asignación de juego
				
				establecer_datos_juego(pack.payload);
				imprime_datos_juego();
			}
			else if(strcmp(pack.type, "00") == 0){ // Mensaje de recepción

				printf("%s \n", pack.payload);
			}
			else if(strcmp(pack.type, "FB") == 0){ // Confirmación de creación de juego

				printf("%s \n", pack.payload);	
				imprime_datos_juego();
			}
		}
	}	

 	printf ("\n Esperando rival... \n");

 	pack = read_package(pack, fifo_input); // Mensaje de inicio de juego
 	printf("%s \n", pack.payload);
 	printf("Prepare sus barcos... \n");	

 	// Posicionamiento de barcos
	int continuar = 0;
	Posicion pos[juegoActual.t1+juegoActual.t2+juegoActual.t3];

	int i,j,k;
	int celdas = 0;
	int valTmp = 0;
	for(i = 0;i < juegoActual.t1; i++){
		char messageToShow[100] = "";
		clear();			
		int op = juegoActual.t1 - i;
		snprintf(messageToShow, 100, "Posicionando barcos... Le restan %d barcos de 1x1",op);
		pos[i] = matrix_game(juegoActual.dimension, pos, celdas, messageToShow);		
		celdas++;
	}

	int j2;	
	for(j = 0;j < juegoActual.t2; j++){		
		for(j2 = 0; j2 < 2; j2++){
			char messageToShow2[100] = "";
			clear();
			int op2 = juegoActual.t2 - j;
			snprintf(messageToShow2, 100, "Posicionando barcos... Le restan %d barcos de 2x1 (H/V)",op2);
			pos[celdas] = matrix_game(juegoActual.dimension, pos, celdas, messageToShow2);
			celdas++;
		}		
	}

	int k2;
	for(k = 0; k < juegoActual.t3; k++){
		for(k2 = 0; k2 < 3; k2++){
			char messageToShow3[100] = "";
			clear();
			int op3 = juegoActual.t3 - k;
			snprintf(messageToShow3, 100, "Posicionando barcos... Le restan %d barcos de 3x1 (H/V)", op3);
			pos[celdas] = matrix_game(juegoActual.dimension, pos, celdas, messageToShow3);
			celdas++;
		}
	}	

	int index_barcos;
	char messagePositionsBef[255];
	char messagePositions[255] = "";
	
	for(index_barcos = 0; index_barcos < celdas; index_barcos++){
		char tempMessage[8];
		sprintf(tempMessage, "%d,%d,",pos[index_barcos].x,pos[index_barcos].y);
		strcat(messagePositions,tempMessage);
	}

	// Envío de posiciones de barcos
	
	pack = build_package(pack, "-", "02", "10", messagePositions);
	send_package(pack, fifo_output);

	// Fin de posicionamiento de barcos
	Posicion ataqueVacio;
	printf ("Esperando su turno... \n");
	//temporal_game(juegoActual.dimension, pos, ataqueVacio, celdas, "Esperando al otro rival...");
	
	int client_out = 0;

	while(client_out != 1){

		char messageToDisplay[255] = "";
	 	pack = read_package(pack, fifo_input); // Mensaje de inicio de juego
 		//if(strcmp(pack.type,"03") == 0)
	 	//	strcpy(messageToDisplay, pack.payload); //printf("%s \n", pack.payload);

		
		//printf("Ingrese numero para enviar \n");
		//scanf("%d",&client_out);
		Posicion ultimo_ataque;
		Posicion p = matrix_game_playing(juegoActual.dimension, pos, ultimo_ataque, celdas, pack.payload);

		pack = build_package(pack, "-", "04", "10", "Ataque en 0,1 ");
		send_package(pack, fifo_output);
		//matrix_game_playing(juegoActual.dimension, pos, ultimo_ataque, celdas, "Esperando turno...");
		printf("Esperando turno... \n");

			    
	}

	close(fifo_output);

}


void main()
{
	 	int fifo_server,fifo_client, fifo;
	 	char *buf;
	 	package packet;
	 	int choice = getpid();
	 	char tmp_pid[8] = "";
	 	char direccion[32] = "/tmp/i_";
	 	
	 	/*      *********************handshake**********************  	*/

	 	printf("Estableciendo conexion... \n");	 	
	 	 
	 	
	 	fifo_server = open_wendy(fifo_server);		
		getAddress(choice, direccion);	
		
		sprintf(tmp_pid,"%d",choice);
		create_input_pipe(direccion);
		

		packet = build_package(packet, "00", "FO", "10", tmp_pid);
		send_package(packet, fifo_server);

	 	fifo_client  = open_input_pipe(direccion,fifo_client);	 		
	 	
	  	packet =  read_package(packet, fifo_client);
	 	printf("Respuesta del servidor: %s", packet.payload);
		

	 	/*      *********************Ingreso al menú principal**********************  	*/
	 	
	 	manage_game(fifo, choice, fifo_client);



	 	close(fifo_server);
	 	close(fifo_client);
}