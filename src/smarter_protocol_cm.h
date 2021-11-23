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
   void connect_to_SAIS(const QString& ip, quint16 port,
                        quint16 local_port);
   void disconnect_from_SAIS();

   void read_SAIS_status();
   void read_haptic_conf_for_dof_id(DOF_Id dof_id);

   void write_SAIS_status(SAIS_Status status);
   void write_haptic_conf(const QJsonDocument& json_doc);

   void command_position(DOF_Id dof_id, DOF_Type dof_type,
                         int pos);
signals:

   void smarter_protocol_cm_msg(const QString& msg);

   void msg_SAIS_request_ok(smarter_msg_ok msg);
   void msg_SAIS_request_failed(smarter_msg_fail msg);

   void msg_SAIS_status(smarter_msg_status msg);

   void msg_SAIS_msg1_state(smarter_msg1_state msg);
   void msg_SAIS_msg2_state(smarter_msg2_state msg);
   void msg_SAIS_msg3_state(smarter_msg3_state msg);
   void msg_SAIS_msg4_state(smarter_msg4_state msg);
   void msg_SAIS_4dof(smarter_msg_4dof msg);

   void msg_SAIS_buttons(smarter_msg_buttons msg);


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
