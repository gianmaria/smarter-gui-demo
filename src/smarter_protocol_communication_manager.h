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

   explicit Smarter_Protocol_Communication_Manager(QObject *parent = nullptr);

   void connect_to_SAIS(const QString& ip, quint16 port, quint16 local_port);
   void request_SAIS_status();
   void set_SAIS_status(int status);
   QString status_to_str(int status_) const;

signals:

   void socket_msg(QString msg);

   void SAIS_status(smarter_msg_status msg);
   void SAIS_4dof(smarter_msg_4dof msg);
   void SAIS_buttons(smarter_msg_buttons msg);
   void SAIS_request_failed(smarter_msg_fail msg);
   void SAIS_request_ok(smarter_msg_ok msg);

private:

   QUdpSocket* udp_socket = nullptr;

private:

   void send_smarter_msg(smarter_msg_id msg_id, void* msg);
   void recv_smarter_msg();

   QString msg_id_to_str(smarter_msg_id msg_id) const;

};

