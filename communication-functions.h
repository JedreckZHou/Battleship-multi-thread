typedef struct pack_prot
{
    char id[32];
    char type[32];
	char size[32];
	char payload[400];
    char checksum[256];    
}package;


package build_package(package packet, char id[32], char type[32], char size[32], char payload[400]){
	strcpy(packet.id, id);
	strcpy(packet.type, type);
	strcpy(packet.size, size);
	strcpy(packet.payload, payload);

	return packet;
}

void send_package(package packet, int fifo){

	write(fifo,&packet.id,32*sizeof(char));
	write(fifo,&packet.type,32*sizeof(char));
	write(fifo,&packet.size,32*sizeof(char));
	write(fifo,&packet.payload,400*sizeof(char));
}

package read_package(package packet, int fifo){

	char tmp_id[32], tmp_size[32], tmp_type[32], tmp_payload[400] ;
	

	read(fifo,&tmp_id,32*sizeof(char));
	strcpy(packet.id, tmp_id);
	read(fifo,&tmp_type,32*sizeof(char));
	strcpy(packet.type, tmp_type);
	read(fifo,&packet.size,32*sizeof(char));
	strcpy(packet.size, tmp_size);
	read(fifo,&tmp_payload,400*sizeof(char));
	strcpy(packet.payload, tmp_payload);
	
	return packet;
}