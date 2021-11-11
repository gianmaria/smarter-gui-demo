/*
This piece of software was developed in the framework of the SMARTER H2020 CleanSky 2 project
Any unauthorized use outside the scope of this project is expressely forbidden

TODO: update this disclaimer and discuss with the consortium and leonardo on sw policies 
*/

#include  "smarter_protocol_streaming.h"

//Global variables
msg_table_entry Registered_Packets[MAX_PACK];
int number_registered_packets = 0; //keep memory of the number of registered packet
//this variable should never be assigned manually!!! the register packet function updates it!

// States definition
typedef enum {
	wait_SYNC1,
	wait_SYNC2,
	wait_ID,
	wait_LENGTH1,
	wait_LENGTH2,
	wait_PAYLOAD,
	wait_CHECKSUM
} waiting_state;


//helper function copy memeory
// this can be implemented differently in different architectures
//
//NOTE: to support C2000, two versions of smarter_memcpy are needed:
// - one that copies from a C-struct to the buffer that is being filled by encode
// - one that copies from the decode message buffer to the C-Struct
#ifndef C2000_VERSION
//fast version using memcpy
void smarter_memcpy(void* dst, const void* src, size_t size) { memcpy(dst, src, size); }
//void smarter_memcpy_struct_to_buff(void* dst, const void* src, size_t size) { memcpy(dst, src, size); }
//void smarter_memcpy_buff_to_struct(void* dst, const void* src, size_t size) { memcpy(dst, src, size); }
//slow version using for cycle
void smarter_memcpy_struct_to_buff(void* dst,  const void* src, size_t size) {
	int i;
	for (i = 0; i < size; i++)
		((unsigned char*)dst)[i] = ((unsigned char*)src)[i];
}
void smarter_memcpy_buff_to_struct(void* dst, const void* src, size_t size) {
	int i;
	for (i = 0; i < size; i++)
		((unsigned char*)dst)[i] = ((unsigned char*)src)[i];
}
#else
//c2000 version copying a byte array into a short-int array adding a padding 0 byte to each short int
//C-Struct to buffer, the struct is accssed byte by byte and each singhle byte is transferred into a single short int of the buffer
//size here is is the width 

void smarter_memcpy_struct_to_buff(int* dst, const int* src, size_t size) {
	int i; //index for destination
	int j = 0; //index for source struct .... it runs at half speed than i
	for (i = 0; i < size; i = i + 2) {
		__byte(&dst[i],0) = __byte(( int* )&src[j], 0);
		__byte(&dst[i+1],0) = __byte(( int* )&src[j], 1);
				j++;
	}
}

void smarter_memcpy_buff_to_struct(int* dst, const int* src, size_t size) {
	int i; //index for source
	int j = 0; //index for destination struct .... it runs at half speed than i
	for (i = 0; i < size; i=i+2) {
        __byte(&dst[j],0) = __byte(( int* )&src[i], 0);
        __byte(&dst[j],1) = __byte(( int* )&src[i+1], 0);
		j++;
	}
}
#endif



smarter_msg_id verify_packet(unsigned char* buff, int size)
{
	//Local variable 
	unsigned int i = 0; //index for the loop
	smarter_msg_length length; //packet length as extracted from the message
	smarter_msg_length pl = 0; //payload length for the received id (from the local message table)
	unsigned char cs = 0;//the receiver must calculate the checksum

	//chcek buff is not NULL (just in case)
	if (buff == NULL) return SMARTER_INVALID_PACKET;

	//ensure size is large enough: size must be at least 2(header)+1(id)+2(length)+1(min payload length)+1(chksum) = 7 
	if (size < 7) return SMARTER_INVALID_PACKET;

	//check header : must be "SM"
	if (buff[0] != 'S') return SMARTER_INVALID_PACKET;
	if (buff[1] != 'M') return SMARTER_INVALID_PACKET;

	//check id 
	//	get length, if length is not -1, than the id belongs to a registered packet 
	pl = get_packet_length(buff[2]);
	if (pl > 0) {
		//here the id is recognized and pl is the expected length of the msg payload
		// => check that the length field inside the received message matches the expected length
#ifndef C2000_VERSION
		__byte(&length, 0) = buff[3]; // ((unsigned char*)(&length))[0] = buff[3];
		__byte(&length, 1) = buff[4]; //((unsigned char*)(&length))[1] = buff[4];
#else
		__byte((int*)&length, 0) = buff[3]; // ((unsigned char*)(&length))[0] = buff[3];
		__byte((int*)&length, 1) = buff[4]; //((unsigned char*)(&length))[1] = buff[4];
#endif
		if (pl != length) {
			//error: packet length does not match the id 
			return SMARTER_INVALID_PACKET;
		}
		//check now that the buff_size is exactly the required message length of this message: 5(header)+1(chk)+length
		if ((pl + 5 + 1) != size) {
			//error: the buffere containing the message is not of the required length .... data corruption ? incomplete packet ? 
			return SMARTER_INVALID_PACKET;
		}
	}
	
	//check chksum
	//cs is the result of xor of all fields of packet, header ^ id ^ length ^ data_payload
	for (i = 0; i < pl+5; i++) cs = cs ^ buff[i];
#ifdef C2000_VERSION
//this is aptch for C2000 version: exclusive or is done between 16 bit chars that have the MSB at 0, thus, after the first XOR the MSB of cs is set to FF
// and compare with buff[5 + pl] fails because it has the MSB at 00 
	cs &= 0x00FF;
#endif
	if (cs!= buff[5 + pl]) return SMARTER_INVALID_PACKET;

	//here the packet is valid => return its id 
	return buff[2];
}


smarter_msg_length get_packet_length(smarter_msg_id id)
{
	//Return the length of the message corresponding to the id
	//Check if id is valid and belongs to the valid list, if true return the corresponding length
	int i;
	for (i = 0; i < number_registered_packets; i++) {
		if (Registered_Packets[i].id == id)	{
			return Registered_Packets[i].length;  
		}
	}
	//if we arrive here, the id is not in the registered packet list => return -1 
	return -1;
}


int decode(const unsigned char* buff, int buff_size, smarter_msg_id id, void* myMsg)
{
	smarter_msg_length size = 0;
	
	//all checks for validity have already been perfomed by verify_packet
	// here id must be valid and buff large enough
	// we just check that buff_size is enough to prevent access to unallocated memory
	// even if this should never happen if this buffer is the same used to call verify_packet

	//check buff is not NULL (just in case)
	if (buff == NULL) return -1;

	//After getting the id, obtain the length of 
	size = get_packet_length(id);

	//check buff is large enough
	if (buff_size < (5 + 1 + size)) {
		//the buffer provided cannot hold the message 
		//this should never happen if the id used here has been provided by verify_packet using the same buffer !!!
		// anyway we do not want memcpy to acces unallocated memory => raise an error
		return -1;
	}

	//copy msg payload into myMsg
#ifndef C2000_VERSION
	smarter_memcpy_buff_to_struct(myMsg, (buff + 5), size);
#else
	smarter_memcpy_buff_to_struct(myMsg, (int*)(buff + 5), size);
#endif
	return size;
}


int encode(unsigned char* buff, int buff_size, smarter_msg_id id, void* packetpointer)
{
	//Local variable for the loop, the get_packet_lenth fun and calculate checksum
	unsigned int i = 0;
	smarter_msg_length size = 0;
	unsigned char cs = 0;

	//check buff is not NULL (just in case)
	if (buff == NULL) return -1;

	//Look for the registered packet that owns that id 
	size = get_packet_length(id);
	//Check if size is -1. The condition is true if there isn't a registered packet that owns that id, so return error (or -1)
	if (size == -1) {
		return -1;
	}
	//check if the buff_size is large enough for this packet 
	// the needed buffer size is 5(header)+1(chk)+size
	if (buff_size < (6 + size)) {
		//the buffer provided is too short => return error
		return -1;
	}
	//Prepare the packet. Build the header locally (for now) that is the same for all packet; 
	buff[0] = 'S'; //1 byte for the char start
	buff[1] = 'M'; // 1 byte for the char start
	buff[2] = id; //1 byte for id
	//encode length in two bytes
	//modified to use the macro byte, it works for both windows and c2000
#ifndef C2000_VERSION
	buff[3] = __byte(&size, 0); //((unsigned char*)(&size))[0];
	buff[4] = __byte(&size, 1); //((unsigned char*)(&size))[1];
#else
	buff[3] = __byte((int*)&size, 0); //((unsigned char*)(&size))[0];
	buff[4] = __byte((int*)&size, 1); //((unsigned char*)(&size))[1];
#endif
	//Copy in buff the payload, considering the header size (the previous 4 bytes) so that buff is ready to be sent
#ifndef C2000_VERSION
	smarter_memcpy_struct_to_buff(buff + 5, packetpointer, size);
#else
	smarter_memcpy_struct_to_buff((int*)buff + 5, (int*)packetpointer, size);
#endif
	//the sender must calculate the checksum. xor alghoritm
	for (i = 0; i < size + 5; i++) cs = cs ^ buff[i];
	//NOTE for C2000, here cs is 16 bit and may have the MSB not 00. This is not a problem since only the LSB is sent !!!!
	buff[5 + size] = cs; //the last byte of packet is the checksum

	//return the whole length of the packet
	return (5 + size + 1);
}

//NOTE: C2000 porting requires that length is always a even number to avoid padding of zeros in the data structure that is actually an array of short int
// when calling register_packet, length MUST BE the length in bytes (both for windows and C2000)
// then register_packet must be called with sizeof()*2 in C2000 !!!!!!
// if the actual length in bytes of the structure (considering 8bit bytes) is not a multiple of two, THIS DOES NOT WORK !!!!!!
boolean register_packet(smarter_msg_id id, smarter_msg_length length) {
	//check if number of the registered packets exceeds the MAX_PACK limit
	//does not check for duplicate messages !! 
	if (number_registered_packets < MAX_PACK) {
		//Fill the Registered_Packets structure with that id and that length
		Registered_Packets[number_registered_packets].id = id;
		Registered_Packets[number_registered_packets].length = length;
		//Increment the variable and return true
		number_registered_packets++;
		return TRUE;
	} else {
		return FALSE;
	}
}

int assemble_packet(unsigned char byte, unsigned char* buff, int buff_size) {
	// The header characters 'SM' identify the packet start. Only these bytes are checked to pass to the next states.
	// a packet is valid if SM and checksum are ok.

	// state machine state initialization 
	static waiting_state wait = wait_SYNC1;
	// variable that stores how many bytes of the packet have been copied into buff
	static int offset = 0;

	static smarter_msg_length payload_length;

	unsigned int i = 0; //index for the loop
	unsigned char cs = 0;// variable for computing checksum

	// check that the buffer is not full, if it happens abort  this packet
	if (offset >= buff_size) {
		// if we get here, it means that the provided buffer buff of size buff_size cannot hold the current packet
		wait = wait_SYNC1;
	}

	// implement the parsing state machine
	switch (wait) {
	case wait_SYNC1:
		if (byte == 'S') {
			wait = wait_SYNC2;
			offset = 0;
			buff[offset++] = byte;
		}
		break;

	case wait_SYNC2:
		if (byte == 'M') {
			wait = wait_ID;
			buff[offset++] = byte;
		}
		else {
			wait = wait_SYNC1;
		}

		break;

	case wait_ID:
		// do not check the id here, it must be checked by verify_packet
		wait = wait_LENGTH1;
		buff[offset++] = byte;
		break;

	case wait_LENGTH1:
		wait = wait_LENGTH2;
		buff[offset++] = byte;
		break;

	case wait_LENGTH2:
		wait = wait_PAYLOAD;
		buff[offset++] = byte;
#ifndef C2000_VERSION
		__byte(&payload_length, 0) = buff[3]; // ((unsigned char*)(&payload_length))[0] = buff[3];
		__byte(&payload_length, 1) = buff[4]; //((unsigned char*)(&payload_length))[1] = buff[4];
#else
		__byte((int*)&payload_length, 0) = buff[3]; // ((unsigned char*)(&payload_length))[0] = buff[3];
		__byte((int*)&payload_length, 1) = buff[4]; //((unsigned char*)(&payload_length))[1] = buff[4];

#endif
		// here payload length contains the length of the payload in the packet
		// if a synchronization error happened payload length maybe very large and irrealistic
		// thus we check here it is not too large (compare against 200, today the largest packet is 107 bytes)
		if ((payload_length > 200) || (payload_length <= 0)) {
			//payload length is irrealistic => abort
			wait = wait_SYNC1;
		}
		break;
	
	case wait_PAYLOAD:
		// store byte 
		// NOTE : this works only if payload length is at least one
		buff[offset++] = byte;
		if (offset == payload_length + 5) {
			//payload is complete => check checksum
			wait = wait_CHECKSUM;			
		} else {
			//payload is not complete => stay in wait_PAYLOAD
			;
		}
		break;

	case wait_CHECKSUM:
		// store byte 
		// NOTE : this works only if payload length is at least one
		buff[offset++] = byte;
		for (i = 0; i < payload_length + 5; i++) cs = cs ^ buff[i];
#ifdef C2000_VERSION
		//this is aptch for C2000 version: exclusive or is done between 16 bit chars that have the MSB at 0, thus, after the first XOR the MSB of cs is set to FF
		// and compare with byte fails because it has the MSB at 00 
		cs &= 0x00FF;
#endif
		if (cs == byte) {
			//checksum ok => returns the number of bytes that filled the buffer buff
			wait = wait_SYNC1;
			return offset;
		} else {
			//checksum is wrong => returns -1
			;
		}
		wait = wait_SYNC1;
		break;
		

	}
	return -1;

}
