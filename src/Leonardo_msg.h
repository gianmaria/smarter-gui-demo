#ifndef __LEONARDO_MSG_H__
#define __LEONARDO_MSG_H__

#include "smarter_protocol_streaming.h"

#define BASE_ID 75
#define LEONARDO_ID BASE_ID+0

//this version is modified to support C2000 architecture that does not have 8-bit data types

#ifndef C2000_VERSION
//set structure alignment to 1 byte !!!!
// this is fundamental to allow porting this library between different compilers and hardware platforms
#pragma pack(push,1) 
#else
#pragma DATA_ALIGN(push,1)
#endif
//flight status from Leonardo simulator 
#define SMARTER_MSG_FLIGHT_STATUS_ID (smarter_msg_id)LEONARDO_ID
typedef struct struct_flight_data {
	SM_int32 pos[3]; // cartesian coordinates (x,y,z) in m/1000
	SM_int16 vel[3]; //linear velocity (in body frame or in navigation frame) in (m/s)/1000
	SM_int16 acc[3];// linear acceleration (in body frame) in (m/s^2)/1000
	SM_int16 ang_vel[3];// angular velocity in body frame (deg/s)/50
	SM_int16 euler[3];// euler angle (roll, pitch, yaw) in (deg/100)
}flight_data;

typedef struct struct_smarter_msg_flight_status {
	SM_uint32 timestamp; // us
	flight_data data;
}smarter_msg_flight_status;


#ifndef C2000_VERSION
#pragma pack(pop)
#endif

#endif