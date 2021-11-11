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

   root["dof"] = msg.dof;

   QJsonObject ss_table;
   ss_table["dof_type"] = msg.ss_table.dof_type;
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

   root["dof"] = msg.dof;

   QJsonObject zg_table;
   zg_table["dof_type"] = msg.zg_table.dof_type;
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

Haptic_Config_Json_Res
verify_haptic_config_json(const QJsonDocument& doc)
{
   auto err = [] (const QString& msg)
   {
      return Haptic_Config_Json_Res{false, msg};
   };

   auto key_exist_and_is_type = [] (const QJsonObject& json,
                                const QString& key,
                                QJsonValue::Type type)
   {
      auto contains = json.contains(key);
      auto is_type = json[key].type() == type;

      return contains && is_type;
   };


   if (doc.isNull())
   {
      return err("Json Document is empty!");
   }

   if (!doc.isObject())
   {
      return err("Json Document is not a json object!");
   }

   const auto& json = doc.object();

   if (!key_exist_and_is_type(json,
                              "haptic_configuration_type",
                              QJsonValue::String))
   {
      return err("key 'haptic_configuration_type'"
                 "does not exist or is not a string!");
   }

   if (json["haptic_configuration_type"].toString()
       == "SS")
   {
      if (key_exist_and_is_type(json, "dof",
                                QJsonValue::Double))
      {
         DOF_Id dof_id = static_cast<DOF_Id>(json["dof"].toDouble());

         if (dof_id != DOF_Id::ROLL &&
             dof_id != DOF_Id::PITCH &&
             dof_id != DOF_Id::YAW)
         {
            return err("key 'dof' is not valid");
         }
      }
      else
      {
         return err("key 'dof' does not exist or is not a number");
      }

      if (!key_exist_and_is_type(json, "ss_table",
                                 QJsonValue::Object))
      {
         return err("key 'ss_table' does not exist or is not an object");
      }

      const auto& ss_table = json["ss_table"].toObject();

      if (!key_exist_and_is_type(ss_table, "damping",
                                 QJsonValue::Double))
      {
         if (ss_table["damping"].toDouble() < 0.0)
         {
            return err("key 'damping' can only be positive");
         }
      }
      else
      {
         return err("key 'damping' does not exist or is not a number");
      }

      if (key_exist_and_is_type(ss_table, "pos_STOP_negative",
                                 QJsonValue::Double))
      {
         if (ss_table["pos_STOP_negative"].toDouble() > 0.0)
         {
            return err("key 'pos_STOP_negative' must be nagative");
         }
      }
      else
      {
         return err("key 'pos_STOP_negative' does not exist or is not a number");
      }


      if (key_exist_and_is_type(ss_table, "pos_STOP_positive",
                                 QJsonValue::Double))
      {
         if (ss_table["pos_STOP_positive"].toDouble() < 0.0)
         {
            return err("key 'pos_STOP_positive' must be positive");
         }
      }
      else
      {
         return err("key 'pos_STOP_positive' does not exist or is not a number");
      }

      if (key_exist_and_is_type(ss_table, "force_BO",
                                 QJsonValue::Double))
      {
         if (ss_table["force_BO"].toDouble() < 0.0)
         {
            return err("key 'force_BO' must be positive");
         }
      }
      else
      {
         return err("key 'force_BO' does not exist or is not a number");
      }

      if (!key_exist_and_is_type(ss_table, "pos_BO",
                                 QJsonValue::Double))
      {
         return err("key 'force_BO' does not exist or is not a number");
      }

      if (key_exist_and_is_type(ss_table, "dof_type",
                                QJsonValue::Double))
      {
         DOF_Type dof_type = static_cast<DOF_Type>(ss_table["dof_type"].toDouble());

         if (dof_type != DOF_Type::LINEAR &&
             dof_type != DOF_Type::ROTATIONAL)
         {
            return err("key 'dof_type' is not valid");
         }
      }
      else
      {
         return err("key 'dof_type' does not exist or is not a number");
      }

      if (!key_exist_and_is_type(json, "pg_negative",
                                 QJsonValue::Array))
      {
         return err("key 'pg_negative' does not exist or is not an array");
      }

      if (!key_exist_and_is_type(json, "pg_positive",
                                 QJsonValue::Array))
      {
         return err("key 'pg_positive' does not exist or is not an array");
      }


      {
         const auto& pg_negative = json["pg_negative"].toArray();

         if (pg_negative.count() != NUMBER_POINTS)
         {
            return err(QString("key 'pg_negative' does not contains the correct number of elements,"
                               "expected %1 found: %2")
                       .arg(NUMBER_POINTS)
                       .arg(pg_negative.count()));
         }

         for (const auto& elem : pg_negative)
         {
            if (!elem.isObject())
               return err("elements inside array 'pg_negative' are not all object!");

            const auto& elem_obj = elem.toObject();

            if (!key_exist_and_is_type(elem_obj, "grad",
                                       QJsonValue::Double))
            {
               return err("key 'grad' does not exist or is not a number");
            }

            if (!key_exist_and_is_type(elem_obj, "pos",
                                       QJsonValue::Double))
            {
               return err("key 'pos' does not exist or is not a number");
            }

         }

      }

      {
         const auto& pg_positive = json["pg_positive"].toArray();

         if (pg_positive.count() != NUMBER_POINTS)
         {
            return err(QString("key 'pg_positive' does not contains the correct number of elements,"
                               "expected %1 found: %2")
                       .arg(NUMBER_POINTS)
                       .arg(pg_positive.count()));
         }

         for (const auto& elem : pg_positive)
         {
            if (!elem.isObject())
               return err("elements inside array 'pg_positive' are not all object!");

            const auto& elem_obj = elem.toObject();

            if (!key_exist_and_is_type(elem_obj, "grad",
                                       QJsonValue::Double))
            {
               return err("key 'grad' does not exist or is not a number");
            }

            if (!key_exist_and_is_type(elem_obj, "pos",
                                       QJsonValue::Double))
            {
               return err("key 'pos' does not exist or is not a number");
            }

         }

      }


   }
   else if (json["haptic_configuration_type"].toString()
            == "ZG")
   {
      if (key_exist_and_is_type(json, "dof",
                                QJsonValue::Double))
      {
         DOF_Id dof_id = static_cast<DOF_Id>(json["dof"].toDouble());

         if (dof_id != DOF_Id::ROLL &&
             dof_id != DOF_Id::PITCH &&
             dof_id != DOF_Id::YAW)
         {
            return err("key 'dof' is not valid");
         }
      }
      else
      {
         return err("key 'dof' does not exist or is not a number");
      }

      if (!key_exist_and_is_type(json, "zg_table",
                                 QJsonValue::Object))
      {
         return err("key 'zg_table' does not exist or is not an object");
      }

      const auto& zg_table = json["zg_table"].toObject();

      if (!key_exist_and_is_type(zg_table, "damping",
                                 QJsonValue::Double))
      {
         return err("key 'damping' does not exist or is not a number");
      }

      if (zg_table["damping"].toDouble() < 0.0)
      {
         return err("key 'damping' can only be positive");
      }

      if (!key_exist_and_is_type(zg_table, "pos_stop1",
                                 QJsonValue::Double))
      {
         return err("key 'pos_stop1' does not exist or is not a number");
      }

      if (!key_exist_and_is_type(zg_table, "pos_stop2",
                                 QJsonValue::Double))
      {
         return err("key 'pos_stop2' does not exist or is not a number");
      }

      if (key_exist_and_is_type(zg_table, "dof_type",
                                QJsonValue::Double))
      {
         DOF_Type dof_type = static_cast<DOF_Type>(zg_table["dof_type"].toDouble());

         if (dof_type != DOF_Type::LINEAR &&
             dof_type != DOF_Type::ROTATIONAL)
         {
            return err("key 'dof_type' is not valid");
         }
      }
      else
      {
         return err("key 'dof_type' does not exist or is not a number");
      }

      if (!key_exist_and_is_type(json, "ga",
                                 QJsonValue::Array))
      {
         return err("key 'ga' does not exist or is not an array");
      }

      if (!key_exist_and_is_type(json, "de",
                                 QJsonValue::Array))
      {
         return err("key 'de' does not exist or is not an array");
      }

      {
         const auto& ga = json["ga"].toArray();

         if (ga.count() != NUMBER_POINTS)
         {
            return err(QString("key 'ga' does not contains the correct number of elements,"
                               "expected %1 found: %2")
                       .arg(NUMBER_POINTS)
                       .arg(ga.count()));
         }

         for (const auto& elem : ga)
         {
            if (!elem.isObject())
               return err("elements inside array 'ga' are not all object!");

            const auto& elem_obj = elem.toObject();

            if (!key_exist_and_is_type(elem_obj, "force",
                                       QJsonValue::Double))
            {
               return err("key 'force' does not exist or is not a number");
            }

            if (!key_exist_and_is_type(elem_obj, "pos",
                                       QJsonValue::Double))
            {
               return err("key 'pos' does not exist or is not a number");
            }

         }

      }

      {
         const auto& de = json["de"].toArray();

         if (de.count() != NUMBER_POINTS)
         {
            return err(QString("key 'de' does not contains the correct number of elements,"
                               "expected %1 found: %2")
                       .arg(NUMBER_POINTS)
                       .arg(de.count()));
         }

         for (const auto& elem : de)
         {
            if (!elem.isObject())
               return err("elements inside array 'de' are not all object!");

            const auto& elem_obj = elem.toObject();

            if (!key_exist_and_is_type(elem_obj, "force",
                                       QJsonValue::Double))
            {
               return err("key 'force' does not exist or is not a number");
            }

            if (!key_exist_and_is_type(elem_obj, "pos",
                                       QJsonValue::Double))
            {
               return err("key 'pos' does not exist or is not a number");
            }

         }

      }

   }
   else
   {
      return err(QString("key 'haptic_configuration_type' has an unknown value, "
                         "expected ZG os SS, found '%1'")
                 .arg(json["haptic_configuration_type"].toString()));
   }

   // TODO: safety mechanism while developing
   return {false, "set back to true"};
}

smarter_msg_write_ss
build_msg_write_ss_from_json_doc(const QJsonDocument& doc)
{
   const QJsonObject& json = doc.object();

   smarter_msg_write_ss msg = {};

   msg.timestamp = 0;
   msg.request_code = SMARTER_MSG_WRITE_SS_ID;
   msg.dof = json["dof"].toInt();
   msg.dof_table = 0; // NOTE: 0 means active configuration and not stored

   const QJsonObject& ss_table = json["ss_table"].toObject();

   msg.ss_table.dof_type = ss_table["dof_type"].toInt();
   msg.ss_table.force_BO = ss_table["force_BO"].toInt();
   msg.ss_table.pos_BO = ss_table["pos_BO"].toInt();
   msg.ss_table.pos_STOP_positive = ss_table["pos_STOP_positive"].toInt();
   msg.ss_table.pos_STOP_negative = ss_table["pos_STOP_negative"].toInt();
   msg.ss_table.damping = ss_table["damping"].toInt();

   const QJsonArray& pg_positive =
         ss_table["pg_positive"].toArray();

   for (auto i = 0;
        i < pg_positive.count();
        ++i)
   {
      const auto& elem = pg_positive[i].toObject();

      msg.ss_table.pg_positive[i].grad = elem["grad"].toInt();
      msg.ss_table.pg_positive[i].pos = elem["pos"].toInt();
   }

   const QJsonArray& pg_negative =
         ss_table["pg_negative"].toArray();

   for (auto i = 0;
        i < pg_negative.count();
        ++i)
   {
      const auto& elem = pg_negative[i];

      msg.ss_table.pg_negative[i].grad = elem["grad"].toInt();
      msg.ss_table.pg_negative[i].pos = elem["pos"].toInt();
   }

   return msg;
}

smarter_msg_write_zg
build_msg_write_zg_from_json_doc(const QJsonDocument& doc)
{
   const QJsonObject& json = doc.object();

   smarter_msg_write_zg msg = {};

   msg.timestamp = 0;
   msg.request_code = SMARTER_MSG_WRITE_ZG_ID;
   msg.dof = json["dof"].toInt();
   msg.dof_table = 0; // NOTE: 0 means active configuration and not stored

   const QJsonObject& zg_table = json["zg_table"].toObject();

   msg.zg_table.dof_type = zg_table["dof_type"].toInt();
   msg.zg_table.damping = zg_table["damping"].toInt();
   msg.zg_table.pos_stop1 = zg_table["pos_stop1"].toInt();
   msg.zg_table.pos_stop2 = zg_table["pos_stop2"].toInt();

   const QJsonArray& de =
         zg_table["de"].toArray();

   for (auto i = 0;
        i < de.count();
        ++i)
   {
      const auto& elem = de[i].toObject();

      msg.zg_table.de[i].pos = elem["pos"].toInt();
      msg.zg_table.de[i].force = elem["force"].toInt();
   }

   const QJsonArray& ga =
         zg_table["ga"].toArray();

   for (auto i = 0;
        i < ga.count();
        ++i)
   {
      const auto& elem = ga[i].toObject();

      msg.zg_table.ga[i].pos = elem["pos"].toInt();
      msg.zg_table.ga[i].force = elem["force"].toInt();
   }

   return msg;
}
