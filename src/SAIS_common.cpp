#include "SAIS_common.h"

#include <QJsonArray>

QString to_str(DOF_Type type)
{
   switch (type)
   {
      case DOF_Type::LINEAR:
         return "LINEAR";
      case DOF_Type::ROTATIONAL:
         return "ROTATIONAL";
      default:
         return "INVALID DOF_Type";
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
      return "INVALID SAIS_Status";
   }
}


QString to_str(DOF_Id id)
{
   switch(id)
   {
      case DOF_Id::ROLL:
      return "Axis 0 (ROLL)";
      case DOF_Id::PITCH:
      return "Axis 1 (PITCH)";
      case DOF_Id::YAW:
      return "Axis 2 (YAW)";
      default:
      return "INVALID DOF_Id";
   }

}

QString smarter_msg_id_to_str(smarter_msg_id msg_id)
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

QJsonDocument to_json(smarter_msg_ss msg)
{
   QJsonObject root;

   root["haptic_configuration_type"] = "SS";

   root["dof"] = to_str(static_cast<DOF_Id>
                        (msg.dof));

   QJsonObject ss_table;
   ss_table["dof_type"] = to_str(static_cast<DOF_Type>
                                 (msg.ss_table.dof_type));
   ss_table["force_BO"] = msg.ss_table.force_BO;
   ss_table["pos_BO"] = msg.ss_table.pos_BO;
   ss_table["pos_STOP_positive"] = msg.ss_table.pos_STOP_positive;
   ss_table["pos_STOP_negative"] = msg.ss_table.pos_STOP_negative;
   ss_table["damping"] = msg.ss_table.damping;

   root["ss_table"] = ss_table;

   QJsonArray pg_negative;

   for (unsigned i = 0;
        i < NUMBER_POINTS;
        ++i)
   {
      QJsonObject pos_grad;
      pos_grad["pos"] = msg.ss_table.pg_negative[i].pos;
      pos_grad["grad"] = msg.ss_table.pg_negative[i].grad;

      pg_negative.append(pos_grad);
   }

   root["pg_negative"] = pg_negative;

   QJsonArray pg_positive;

   for (unsigned i = 0;
        i < NUMBER_POINTS;
        ++i)
   {
      QJsonObject pos_grad;
      pos_grad["pos"] = msg.ss_table.pg_positive[i].pos;
      pos_grad["grad"] = msg.ss_table.pg_positive[i].grad;

      pg_positive.append(pos_grad);
   }

   root["pg_positive"] = pg_positive;

   return QJsonDocument(root);
}


QJsonDocument to_json(smarter_msg_zg msg)
{
   QJsonObject root;

   root["haptic_configuration_type"] = "ZG";

   root["dof"] = to_str(static_cast<DOF_Id>
                        (msg.dof));

   QJsonObject zg_table;
   zg_table["dof_type"] = to_str(static_cast<DOF_Type>
                                 (msg.zg_table.dof_type));
   zg_table["damping"] = msg.zg_table.damping;
   zg_table["pos_stop1"] = msg.zg_table.pos_stop1;
   zg_table["pos_stop2"] = msg.zg_table.pos_stop2;

   root["zg_table"] = zg_table;

   QJsonArray de;

   for (unsigned i = 0;
        i < NUMBER_POINTS;
        ++i)
   {
      QJsonObject detent;
      detent["pos"]   = msg.zg_table.de[i].pos;
      detent["force"] = msg.zg_table.de[i].force;

      de.append(detent);
   }

   root["de"] = de;

   QJsonArray ga;

   for (unsigned i = 0;
        i < NUMBER_POINTS;
        ++i)
   {
      QJsonObject gate;
      gate["pos"]   = msg.zg_table.ga[i].pos;
      gate["force"] = msg.zg_table.ga[i].force;

      ga.append(gate);
   }

   root["ga"] = ga;

   return QJsonDocument(root);
}
