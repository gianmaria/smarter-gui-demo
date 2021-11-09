#pragma once

#include <QObject>
#include <QUdpSocket>

#include "SAIS_common.h"

extern "C"
{
#include "smarter_protocol_streaming.h"
}

class Smarter_Protocol_CM : public QObject
{
   Q_OBJECT

public:

   explicit Smarter_Protocol_CM(QObject *parent = nullptr);

public slots:
   void connect_to_SAIS(const QString& ip, quint16 port, quint16 local_port);

   void read_SAIS_status();
   void read_haptic_conf_for_dof_id(unsigned short dof_id);

   void set_SAIS_status(SAIS_Status status);

signals:

   void socket_msg(QString msg);

   void mag_SAIS_status(smarter_msg_status msg);
   void msg_SAIS_4dof(smarter_msg_4dof msg);
   void msg_SAIS_buttons(smarter_msg_buttons msg);
   void msg_SAIS_request_failed(smarter_msg_fail msg);
   void msg_SAIS_request_ok(smarter_msg_ok msg);

   // Haptic Configuration for single dof of SS type.
   void msg_SAIS_haptic_conf_ss(smarter_msg_ss msg);
   // Haptic Configuration for single dof of ZG type.
   void msg_SAIS_haptic_conf_zg(smarter_msg_zg msg);

private:

   QUdpSocket* udp_socket = nullptr;

private:

   void send_smarter_msg(smarter_msg_id msg_id, void* msg);
   void recv_smarter_msg();
};

typedef Smarter_Protocol_CM SmarterPCM;
