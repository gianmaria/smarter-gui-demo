/*
This piece of software was developed in the framework of the SMARTER H2020 CleanSky 2 project
Any unauthorized use outside the scope of this project is expressely forbidden

TODO: update this disclaimer and discuss with the consortium and leonardo on sw policies
*/


#ifndef __SMARTER_H__
#define __SMARTER_H__

const char* smarter_protocol_streaming_version = "2.0.1";

//this version is modified to support C2000 architecture that does not have 8-bit data types

//define the following macro when compiling on C2000
//#define C2000_VERSION

#ifndef C2000_VERSION
#include <Windows.h>
//this is a windows version of the c2000 __byte macro
#define __byte(B,P) ((unsigned char*)(B))[P]
//
//define types and macros
//
#define TRUE            1
#define FALSE           0

#define SM_char short int
#define SM_uchar unsigned short int
#define SM_int16 short int
#define SM_uint16 unsigned short int
#define SM_int32 int
#define SM_uint32 unsigned int


#else
#include "GlobalTypes.h"
#define size_t short
#define boolean Bool
#define NULL 0


//
//define types and macros
//
#define TRUE            True
#define FALSE           False

#define SM_char short int
#define SM_uchar unsigned short int
#define SM_int16 short int
#define SM_uint16 unsigned short int
#define SM_int32 long
#define SM_uint32 unsigned long

#endif


//valid message IDs range 1-255, msg_id = 0 is used as invalid_packet
//NOTE: in C2000 this becomes a short int but ONLY THE LOWEST 8 BITS are considered !!!! Do not use ids larger than 255
typedef SM_uchar smarter_msg_id;
#define SMARTER_INVALID_PACKET  0

//the message length is encoded as a 16 bit signed integer (=> max packet length 32767 bytes) 
//TODO: check and ensure here that the platform used compiles short int as 16 bit integers
typedef SM_int16 smarter_msg_length;

//define max number of registrable packets
#define MAX_PACK		100
// this can be implemented differently in different architectures
void smarter_memcpy(void* dst, const void* src, size_t size);


#ifndef C2000_VERSION
//set structure alignment to 1 byte !!!!
// this is fundamental to allow porting this library between different compilers and hardware platforms
#pragma pack(push,1) 
#else

#pragma DATA_ALIGN(push,1)
#endif

//
//  packet structures definition
//



//this struct provides position, velocity and force for single dof
typedef struct struct_pos_vel_force {
	SM_uchar dof_type; //0:linear, 1: rotational 16 bit
	SM_int16 position;// 16 bit
	SM_int16 velocity;// 16 bit
	SM_int16 force;//16 bit
}pos_vel_force;

//SMARTER_msg 1-6: msg for each dof, from SAIS to FCS
#define SMARTER_MSG1_STATE_ID (smarter_msg_id)1
typedef struct struct_smarter_msg1_state {
	SM_uint32 timestamp; // 32 bit us
	pos_vel_force pvf; //16*4 bit
} smarter_msg1_state;

#define SMARTER_MSG2_STATE_ID (smarter_msg_id)2
typedef struct struct_smarter_msg2_state {
	SM_uint32 timestamp; // 32 bit, us
	pos_vel_force pvf;//16*4 bit
} smarter_msg2_state;

#define SMARTER_MSG3_STATE_ID (smarter_msg_id)3
typedef struct struct_smarter_msg3_state{
	SM_uint32 timestamp; // 32 bit, us
	pos_vel_force pvf;//16*4 bit
} smarter_msg3_state;

#define SMARTER_MSG4_STATE_ID (smarter_msg_id)4
typedef struct struct_smarter_msg4_state{
	SM_uint32 timestamp; // 32 bit, us
	pos_vel_force pvf;//16*4 bit
} smarter_msg4_state;

#define SMARTER_MSG5_STATE_ID (smarter_msg_id)5
typedef struct struct_smarter_msg5_state {
	SM_uint32 timestamp; // 32 bit, us
	pos_vel_force pvf;//16*4 bit
} smarter_msg5_state;

#define SMARTER_MSG6_STATE_ID (smarter_msg_id)6
typedef struct struct_smarter_msg6_state {
	SM_uint32 timestamp; // 32 bit, us
	pos_vel_force pvf;//16*4 bit
} smarter_msg6_state;

//SMARTER_msg7: message for buttons state, from SAIS to FCS
#define SMARTER_MSG_BUTTONS_ID (smarter_msg_id)7
typedef struct struct_smarter_msg_buttons {
	SM_uint32 timestamp; // 32 bit, us
	SM_uint16 buttons_state;// 16 bit
   SM_int16 axis1; // pads, value from -100 to 100 %
   SM_int16 axis2;
   SM_int16 axis3;
   SM_int16 axis4;
} smarter_msg_buttons;

//SMARTER_msg8 : message for 4 dof + buttons state, from SAIS to FCS
#define SMARTER_MSG_4DOF_ID (smarter_msg_id)8
typedef struct struct_smarter_msg_4dof {
	SM_uint32 timestamp; // 32 bit, us
	pos_vel_force pvf[4];// 4*64 bit
	SM_uint16 buttons_state; //16 bit buttons logic state  (HLL = "pressed")
   SM_int16 axis1;
   SM_int16 axis2;
   SM_int16 axis3;
   SM_int16 axis4;
}smarter_msg_4dof;

//SMARTER_msg9 : message for 5 dof + buttons state, from SAIS to FCS
#define SMARTER_MSG_5DOF_ID (smarter_msg_id)9
typedef struct struct_smarter_msg_5dof {
	SM_uint32 timestamp; // 32 bit, us
	pos_vel_force pvf[5];//5*64 bit
	SM_uint16 buttons_state; //16 bit buttons logic state  (HLL = "pressed")
	SM_uint16 axis1;
	SM_uint16 axis2;
	SM_uint16 axis3;
	SM_uint16 axis4;
}smarter_msg_5dof;
//SMARTER_msg10 : message for all dof + buttons state, from SAIS to FCS
#define SMARTER_MSG_6DOF_ID (smarter_msg_id)10
typedef struct struct_smarter_msg_6dof{
	SM_uint32 timestamp; // 32 bit, us
	pos_vel_force pvf[6];//6*64 bit
	SM_uint16 buttons_state; //16 bit buttons logic state  (HLL = "pressed")
	SM_uint16 axis1;
	SM_uint16 axis2;
	SM_uint16 axis3;
	SM_uint16 axis4;
}smarter_msg_6dof;

//SMARTER_msg11 , message from FCS to SAIS to read the haptic configuration of a single dof (active or stored)
#define SMARTER_MSG_READ_ID (smarter_msg_id)11 
typedef struct struct_smarter_msg_read {
	SM_uchar dof;// 16 bit
	SM_uchar dof_table;// 16 bit
	SM_int32 request_code;// 32 bit
}smarter_msg_read;

//SMARTER_msg12: message from SAIS, reply to read command from FCS (SS config). NUMBER_POINTS corresponds to a fixed number of gradient for SS
#define SMARTER_MSG_SS_ID (smarter_msg_id)12
#define NUMBER_POINTS	10
//sub-struct that provides the couple (pos, gradient) 
typedef struct struct_pos_grad {
	SM_int16 pos;//16 bit
	SM_int16 grad;//16 bit
}pos_grad;
//sub-struct that defines simulated spring (ss) 
typedef struct struct_table_ss {
	SM_uchar dof_type;// 16 bit, 0: linear, 1: rotational
	SM_int16 force_BO; //16 bit, breakout force
	SM_int16 pos_BO; //16 bit, breakout position
	SM_int16 pos_STOP_positive;// 16 bit, soft stop position on the right side of BO
	SM_int16 pos_STOP_negative;// 16 bit, soft stop position on the left side of BO
	SM_uint16 damping; //16 bit
	pos_grad pg_positive[NUMBER_POINTS];//32*10 bit
	pos_grad pg_negative[NUMBER_POINTS];//32*10 bit
}table_ss;

typedef struct struct_smarter_msg_ss {
	SM_uint32 timestamp; // 32 bit
	SM_int32 request_code;// 32 bit reply with the same request code
	table_ss ss_table; // 46 short -> 92 byte -> 736 bit
    SM_uchar dof; // 16 bit
}smarter_msg_ss;

//SMARTER_msg13, message from SAIS, reply to read command from FCS (ZG config)
#define SMARTER_MSG_ZG_ID (smarter_msg_id)13
//sub-struct that provides the couple (pos, force)
typedef struct struct_detent {
	SM_int16 pos;//16 bit
	SM_int16 force; // 16 bit
}detent;
//sub-struct that provides the couple (pos, force)  for zg config
typedef struct struct_gate {
	SM_int16 pos;// 16 bit
	SM_int16 force;//16 bit
}gate;
//sub-struct that defines zero gradient (zg) active table
typedef struct struct_table_zg {
	detent de[NUMBER_POINTS]; //32*10 bit
	gate ga[NUMBER_POINTS]; // 32*10 bit
    SM_uchar dof_type; // 16 bit 0: linear, 1:rotational
	SM_uint16 damping;// 16 bit
	SM_int16 pos_stop1;//16 bit, first stop position
	SM_int16 pos_stop2;// 16 bit,second stop position
}table_zg;

typedef struct struct_smarter_msg_zg {
	SM_uint32 timestamp; // 32 bit
	SM_int32 request_code;// 32 bit
	table_zg zg_table; // 44 short -> 88 byte -> 704 bit
    SM_uchar dof; // 16 bit
}smarter_msg_zg;

//SMARTER_msg14, write SS config, sent from FCS to SAIS
#define SMARTER_MSG_WRITE_SS_ID (smarter_msg_id)14
typedef struct struct_smarter_msg_write_ss {
	SM_uint32 timestamp;// 32 bit
	SM_int32 request_code;// 32 bit
	table_ss ss_table; // 46 short -> 92 byte -> 736 bit
    SM_uchar dof; // 16 bit
    SM_uchar dof_table; // 16 bit set active or stored table
}smarter_msg_write_ss;

//SMARTER_msg15, write ZG config, sent from FCS to SAIS
#define SMARTER_MSG_WRITE_ZG_ID (smarter_msg_id)15
typedef struct struct_smarter_msg_write_zg {
	SM_uint32 timestamp; // 32 bit
	SM_int32 request_code; // 32 bit
	table_zg zg_table; // 44 short -> 88 byte -> 704 bit
    SM_uchar dof; // 16 bit
    SM_uchar dof_table;// 16 bit set active or stored table
}smarter_msg_write_zg;

//SMARTER_msg16, ok message from SAIS to FCS
#define SMARTER_MSG_OK_ID (smarter_msg_id)16
typedef struct struct_smarter_msg_ok {
	SM_int32 request_code; // 32 bit
} smarter_msg_ok;

//SMARTER_msg17, FAIL message from SAIS to FCS
#define SMARTER_MSG_FAIL_ID (smarter_msg_id)17
typedef struct struct_smarter_msg_fail {
	SM_int32 request_code;// 32 bit
	SM_int32 error_code; //32 bit
#ifndef C2000_VERSION
	unsigned char error_string[64];// 8*64 bit
#else
	//NOTE: C2000 version requires half of array length since an unsigned char is int16 !!!
    unsigned char error_string[32];// 16*32 bit
#endif
}smarter_msg_fail;

//SMARTER_msg18, set active haptic configuration from memory (SS o ZG of a single dof), from FCS to SAIS
#define SMARTER_MSG_SET_ACTIVE_ID (smarter_msg_id)18 
typedef struct struct_smarter_msg_set_active {
	SM_int32 request_code; // 32 bit
    SM_uchar dof;// 16 bit
    SM_uchar dof_table; // 16 bit
}smarter_msg_set_active;

//SMARTER_msg19, set stop position for SS, from FCS to SAIS
#define SMARTER_MSG_SETSTOP_SS_ID (smarter_msg_id)19
typedef struct struct_smarter_msg_setStop_ss {
	SM_uint32 timestamp;// 32 bit
	SM_int32 request_code;// 32 bit
	SM_uchar dof;// 16 bit
	SM_uchar STOP_id; // 16 bit, 0: stop negative, 1: stop positive
	SM_int16 pos_STOP; //16 bit soft stop position
}smarter_msg_setStop_ss;

//SMARTER_msg20, set stop position for ZG, from FCS to SAIS
#define SMARTER_MSG_SETSTOP_ZG_ID (smarter_msg_id)20
typedef struct struct_smarter_msg_setStop_zg {
	SM_uint32 timestamp;// 32 bit
	SM_int32 request_code;// 32 bit
	SM_uchar dof;// 16 bit
	SM_uchar STOP_id; // 16 bit, 0: stop 1, 1: stop 2
	SM_int16 pos_STOP;//16 bit stop position
}smarter_msg_setStop_zg;

//SMARTER_msg21, set detent position, from FCS to SAIS
#define SMARTER_MSG_SETDETENT_ID (smarter_msg_id)21
typedef struct struct_smarter_msg_setDetent {
	SM_uint32 timestamp;// 32 bit
	SM_int32 request_code;// 32 bit
    detent pos_detent;// 16*2 bit
	SM_uchar dof;// 16 bit
	SM_uchar STOP_id; // 16 bit, identifies the detent position in the array, 0-9  !!!!!!!!!!!! errore nel nome!!!!!!!
}smarter_msg_setDetent;

//SMARTER_msg22, set gate position, from FCS to SAIS
#define SMARTER_MSG_SETGATE_ID (smarter_msg_id)22
typedef struct struct_smarter_msg_setGate {
	SM_uint32 timestamp;// 32 bit
	SM_int32 request_code;// 32 bit
    gate pos_gate;// 16*2 bit
	SM_uchar dof;// 16 bit
	SM_uchar STOP_id;// 16 bit, identifies the detent position, 0-9   !!!!!!!!!!!! errore nel nome!!!!!!!
}smarter_msg_setGate;

//SMARTER_msg23, set damping of rotary motion dof, from FCS to SAIS
#define SMARTER_MSG_SETDAMPING_ROT_ID (smarter_msg_id)23
typedef struct struct_smarter_msg_setDamping_rot {
	SM_uint32 timestamp;// 32 bit
	SM_int32 request_code;// 32 bit
	SM_uchar dof;// 16 bit
	SM_uint16 damping;// 16 bit
}smarter_msg_setDamping_rot;

//SMARTER_msg24, set damping of linear motion dof, from FCS to SAIS
#define SMARTER_MSG_SETDAMPING_LIN_ID (smarter_msg_id)24
typedef struct struct_smarter_msg_setDamping_lin {
	SM_uint32 timestamp;// 32 bit
	SM_int32 request_code;// 32 bit
	SM_uchar dof;// 16 bit
	SM_uint16 damping;// 16 bit
}smarter_msg_setDamping_lin;

//SMARTER_msg25, request desired pos-vel-force references for a single dof, from FCS to SAIS
#define SMARTER_MSG_SEND_REF_ID (smarter_msg_id)25 
typedef struct struct_smarter_msg_send_ref {
	SM_int32 request_code;//32 bit
	pos_vel_force pvf;// 64 bit
    SM_uchar dof;// 16 bit
	SM_uchar pvf_selector;// 16 bit
}smarter_msg_send_ref;


//SMARTER_msg26, request desired pos-vel-force references for 4 dofs, from FCS to SAIS
#define SMARTER_MSG_SEND_REF_4DOF_ID (smarter_msg_id)26
typedef struct struct_smarter_msg_send_ref_4dof {
	SM_int32 request_code;// 32 bit
	pos_vel_force pvf[4];// 64*4 bit
	SM_uchar pvf_selector[4];// 16*4 bit
}smarter_msg_send_ref_4dof;

//SMARTER_msg27, request desired pos-vel-force references for 5 dofs, from FCS to SAIS
#define SMARTER_MSG_SEND_REF_5DOF_ID (smarter_msg_id)27 
typedef struct struct_smarter_msg_send_ref_5dof {
	SM_int32 request_code; // 32 bit
	pos_vel_force pvf[5];// 64*5
	SM_uchar pvf_selector[5];// 16*5
}smarter_msg_send_ref_5dof;


//SMARTER_msg28, request desired pos-vel-force references of 6 dofs from FCS to SAIS
#define SMARTER_MSG_READ_REF_6DOF_ID (smarter_msg_id)28
typedef struct struct_smarter_msg_send_ref_6dof {
	SM_int32 request_code;// 32 bit
	pos_vel_force pvf[6];//64*6
	SM_uchar pvf_selector[6];// 16*6
}smarter_msg_send_ref_6dof;


//SMARTER_msg29, write the status of SAIS, from FCS to SAIS
#define SMARTER_MSG_WRITE_STATUS_ID (smarter_msg_id)29
typedef struct struct_smarter_msg_write_status {
	SM_uint32 timestamp; // 32 bit
	SM_int32 request_code;// 32 bit
	SM_uchar status;// 16 bit 0,1,2 --> ACTIVE, PASSIVE, STANDBY
}smarter_msg_write_status;

//SMARTER_msg30, read status, from FCS to SAIS
#define SMARTER_MSG_READ_STATUS_ID (smarter_msg_id)30
typedef struct struct_smarter_msg_read_status {
	SM_int32 request_code;// 32 bit
}smarter_msg_read_status;

//SMARTER_msg31, status of SAIS, from SAIS to FCS
#define SMARTER_MSG_STATUS_ID (smarter_msg_id)31
typedef struct struct_smarter_msg_status {
	SM_uint32 timestamp;// 32 bit
	SM_int32 request_code;// 32 bit
	SM_uchar status;// 16 bit
}smarter_msg_status;

//SMARTER_msg32, write configuration parameters ( for now only frequency), from FCS to SAIS
#define SMARTER_MSG_WRITE_CONFIG_ID (smarter_msg_id)32
typedef struct struct_smarter_msg_write_config {
	SM_uint32 timestamp;// 32 bit
	SM_int32 request_code;// 32 bit
	SM_uchar frequency;//16 bit
}smarter_msg_write_config;

//SMARTER_msg33, read configuration parameters, from FCS to SAIS
#define SMARTER_MSG_READ_CONFIG_ID (smarter_msg_id)33
typedef struct struct_smarter_msg_read_config {
	SM_int32 request_code;// 32bit
}smarter_msg_read_config;

//SMARTER_msg34, configuration parameters, from SAIS to FCS
#define SMARTER_MSG_CONFIG_ID (smarter_msg_id)34
typedef struct struct_smarter_msg_config {
	SM_uint32 timestamp;// 32 bit us
	SM_int32 request_code;// 32 bit
	SM_uchar frequency;// 16 bit
}smarter_msg_config;



#ifndef C2000_VERSION
#pragma pack(pop)
#endif



//management of valid messages
//valid messages must be registered calling register_packet
//registered messages are stored as id-length couples in msg_table
//	the length is encoded as a 16 bit signed integer (=> max packet length 32767 bytes) 
typedef struct struct_msg_table_entry {
	smarter_msg_id id;
	smarter_msg_length length;
} msg_table_entry;


//
// Function prototypes
//

/*-------------------------------------------------------------------
Smarter packets are composed by:
		 
		 [start][ID][length][payload][checksum]
		   2B    1B   2B	 length		1B

start = "SM"		   
id in the range 1-255 (0 is reserved for the invalid packet code)
length = the length of the payload section
checksum = xor of all packet bytes including header
--------------------------------------------------------------------*/



// checks if the memory buffer contains a valid packet:
//  - valid header
//  - valid id (packet must be registered)
//  - valid length (must match that of the id)
//  - checksum ok
// if ok returns the packet id of the msg contained in the buffer buff (of size size) 
// the packet must be registered otherwise it returns INVALID_PACKET
smarter_msg_id verify_packet(unsigned char* buff, int buff_size);

//get length of packet from id
// if the id is not registered it returns -1
smarter_msg_length get_packet_length(smarter_msg_id); // return the size of the payload that corresponds to that id

//decode the packet contained in the buffer buff (of size buff_size) given the message id
//	myMsg must point to a message structure appropriate for the message id 
//note: this function MUST be called AFTER having called verify_packet!! 
//      for performance reasons, this function DOES NOT check if a packet is valid 
//		calling decode on an invalid packet (not validated by verify_packet) is forbidden!
#ifndef C2000_VERSION
int decode(const unsigned char* buff, int buff_size, smarter_msg_id id, void* myMsg);
#else
int decode(const unsigned char* buff, int buff_size, smarter_msg_id id, int* myMsg);
#endif
//encode into buff the message with message id = smarter_msg_id and pointed by packetpointer 
int encode(unsigned char* buff, int buff_size, smarter_msg_id, void* packetpointer);

// function to register packet
boolean register_packet(smarter_msg_id, smarter_msg_length);

// this function can handle packets split into different udp/ip packets, every byte of the buffer is passed singularly
int assemble_packet(unsigned char byte, unsigned char* buff, int buff_size);

#endif
