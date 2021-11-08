#pragma once

#include <QObject>
#include <QUdpSocket>

extern "C"
{
#include "smarter_protocol_streaming.h"
}

class Smarter_Protocol_Communication_Manager : public QObject
{
   Q_OBJECT

public:

   enum SAIS_Status : SM_uchar
   {
      // ACTIVE (2) , PASSIVE (1) or STANDBY (0)
      STANDBY = 0,
      PASSIVE = 1,
      ACTIVE = 2,
      UNKNOWN
   };

   static QString SAIS_Status_to_str(SAIS_Status status);
   static QString smarter_msg_id_to_str(smarter_msg_id msg_id);

   explicit Smarter_Protocol_Communication_Manager(QObject *parent = nullptr);

   void connect_to_SAIS(const QString& ip, quint16 port, quint16 local_port);
   void read_SAIS_status();
   void set_SAIS_status(SAIS_Status status);


signals:

   void socket_msg(QString msg);

   void mag_SAIS_status(smarter_msg_status msg);
   void msg_SAIS_4dof(smarter_msg_4dof msg);
   void msg_SAIS_buttons(smarter_msg_buttons msg);
   void msg_SAIS_request_failed(smarter_msg_fail msg);
   void msg_SAIS_request_ok(smarter_msg_ok msg);

private:

   QUdpSocket* udp_socket = nullptr;

private:

   void send_smarter_msg(smarter_msg_id msg_id, void* msg);
   void recv_smarter_msg();
};

typedef Smarter_Protocol_Communication_Manager SmarterPCM;
