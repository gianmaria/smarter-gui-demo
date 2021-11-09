#include "SAIS_common.h"

QString to_str(DOF_Type type)
{
   switch (type)
   {
      case DOF_Type::LINEAR:
         return "LINEAR";
      case DOF_Type::ROTATIONAL:
         return "ROTATIONAL";
      default:
         return "UNKNOWN DOF_Type";
   }
}

QString to_str(SAIS_Status status)
{
   // ACTIVE (2) , PASSIVE (1) or STANDBY (0)

   switch (status)
   {
      case SAIS_Status::STANDBY:
      return "STANDBY";
      case SAIS_Status::PASSIVE:
      return "PASSIVE";
      case SAIS_Status::ACTIVE:
      return "ACTIVE";
      default:
      return "UNKNOWN SAIS_Status";
   }
}

QString to_str(smarter_msg_id msg_id)
{
   switch (msg_id)
   {
      case SMARTER_INVALID_PACKET: return "INVALID MSG";
      case SMARTER_MSG1_STATE_ID: return "SMARTER_MSG1_STATE_ID";
      case SMARTER_MSG2_STATE_ID: return "SMARTER_MSG2_STATE_ID";
      case SMARTER_MSG3_STATE_ID: return "SMARTER_MSG3_STATE_ID";
      case SMARTER_MSG4_STATE_ID: return "SMARTER_MSG4_STATE_ID";
      case SMARTER_MSG5_STATE_ID: return "SMARTER_MSG5_STATE_ID";
      case SMARTER_MSG6_STATE_ID: return "SMARTER_MSG6_STATE_ID";
      case SMARTER_MSG_BUTTONS_ID: return "SMARTER_MSG_BUTTONS_ID";
      case SMARTER_MSG_4DOF_ID: return "SMARTER_MSG_4DOF_ID";
      case SMARTER_MSG_5DOF_ID: return "SMARTER_MSG_5DOF_ID";
      case SMARTER_MSG_6DOF_ID: return "SMARTER_MSG_6DOF_ID";
      case SMARTER_MSG_READ_ID: return "SMARTER_MSG_READ_ID";
      case SMARTER_MSG_SS_ID: return "SMARTER_MSG_SS_ID";
      case SMARTER_MSG_ZG_ID: return "SMARTER_MSG_ZG_ID";
      case SMARTER_MSG_WRITE_SS_ID: return "SMARTER_MSG_WRITE_SS_ID";
      case SMARTER_MSG_WRITE_ZG_ID: return "SMARTER_MSG_WRITE_ZG_ID";
      case SMARTER_MSG_OK_ID: return "SMARTER_MSG_OK_ID";
      case SMARTER_MSG_FAIL_ID: return "SMARTER_MSG_FAIL_ID";
      case SMARTER_MSG_SET_ACTIVE_ID: return "SMARTER_MSG_SET_ACTIVE_ID";
      case SMARTER_MSG_SETSTOP_SS_ID: return "SMARTER_MSG_SETSTOP_SS_ID";
      case SMARTER_MSG_SETSTOP_ZG_ID: return "SMARTER_MSG_SETSTOP_ZG_ID";
      case SMARTER_MSG_SETDETENT_ID: return "SMARTER_MSG_SETDETENT_ID";
      case SMARTER_MSG_SETGATE_ID: return "SMARTER_MSG_SETGATE_ID";
      case SMARTER_MSG_SETDAMPING_ROT_ID: return "SMARTER_MSG_SETDAMPING_ROT_ID";
      case SMARTER_MSG_SETDAMPING_LIN_ID: return "SMARTER_MSG_SETDAMPING_LIN_ID";
      case SMARTER_MSG_SEND_REF_ID: return "SMARTER_MSG_SEND_REF_ID";
      case SMARTER_MSG_SEND_REF_4DOF_ID: return "SMARTER_MSG_SEND_REF_4DOF_ID";
      case SMARTER_MSG_SEND_REF_5DOF_ID: return "SMARTER_MSG_SEND_REF_5DOF_ID";
      case SMARTER_MSG_READ_REF_6DOF_ID: return "SMARTER_MSG_READ_REF_6DOF_ID";
      case SMARTER_MSG_WRITE_STATUS_ID: return "SMARTER_MSG_WRITE_STATUS_ID";
      case SMARTER_MSG_READ_STATUS_ID: return "SMARTER_MSG_READ_STATUS_ID";
      case SMARTER_MSG_STATUS_ID: return "SMARTER_MSG_STATUS_ID";
      case SMARTER_MSG_WRITE_CONFIG_ID: return "SMARTER_MSG_WRITE_CONFIG_ID";
      case SMARTER_MSG_READ_CONFIG_ID: return "SMARTER_MSG_READ_CONFIG_ID";
      case SMARTER_MSG_CONFIG_ID: return "SMARTER_MSG_CONFIG_ID";
      default: return "UNKNOWN MSG ID";
   }
}
