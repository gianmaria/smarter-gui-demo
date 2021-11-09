#pragma once

// SAIS -> Smart Active Inceptor Systems

extern "C" {
#include "smarter_protocol_streaming.h"
}

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

enum class DOF_Id : SM_uchar
{
   ROLL = 0,
   PITCH = 1,
   YAW = 2,
   INVALID
};

enum class DOF_Type : SM_uchar
{
   // 0:linear, 1: rotational 16 bit
   LINEAR = 0,
   ROTATIONAL = 1,
   INVALID
};

enum class SAIS_Status : SM_uchar
{
   // ACTIVE (2) , PASSIVE (1) or STANDBY (0)
   STANDBY = 0,
   PASSIVE = 1,
   ACTIVE = 2,
   INVALID
};

QString to_str(DOF_Type type);
QString to_str(SAIS_Status status);
QString to_str(DOF_Id id);
QString smarter_msg_id_to_str(smarter_msg_id msg_id);

QJsonDocument to_json(smarter_msg_ss msg);
QJsonDocument to_json(smarter_msg_zg msg);
