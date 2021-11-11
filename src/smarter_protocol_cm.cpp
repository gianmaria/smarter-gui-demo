#include "smarter_protocol_cm.h"

#include <time.h>

#include <QJsonArray>
#include <QNetworkDatagram>

Smarter_Protocol_CM::Smarter_Protocol_CM(QObject *parent) : QObject(parent)
{
   //register packets that I want to send or read
   register_packet(SMARTER_MSG_OK_ID, sizeof(smarter_msg_ok));
   register_packet(SMARTER_MSG_FAIL_ID, sizeof(smarter_msg_fail));

   register_packet(SMARTER_MSG_WRITE_STATUS_ID, sizeof(smarter_msg_write_status));
   register_packet(SMARTER_MSG_READ_STATUS_ID, sizeof(smarter_msg_read_status));

   register_packet(SMARTER_MSG_STATUS_ID, sizeof(smarter_msg_status));
   register_packet(SMARTER_MSG1_STATE_ID, sizeof(smarter_msg1_state));
   register_packet(SMARTER_MSG2_STATE_ID, sizeof(smarter_msg2_state));
   register_packet(SMARTER_MSG3_STATE_ID, sizeof(smarter_msg3_state));
   register_packet(SMARTER_MSG4_STATE_ID, sizeof(smarter_msg4_state));

   register_packet(SMARTER_MSG_4DOF_ID, sizeof(smarter_msg_4dof));
   register_packet(SMARTER_MSG_5DOF_ID, sizeof(smarter_msg_5dof));
   register_packet(SMARTER_MSG_BUTTONS_ID, sizeof(smarter_msg_buttons));

   register_packet(SMARTER_MSG_SETSTOP_SS_ID, sizeof(smarter_msg_setStop_ss));
   register_packet(SMARTER_MSG_SETSTOP_ZG_ID, sizeof(smarter_msg_setStop_zg));

   register_packet(SMARTER_MSG_READ_ID, sizeof(smarter_msg_read));
   register_packet(SMARTER_MSG_SS_ID, sizeof(smarter_msg_ss));
   register_packet(SMARTER_MSG_ZG_ID, sizeof(smarter_msg_zg));

   register_packet(SMARTER_MSG_WRITE_SS_ID, sizeof(smarter_msg_write_ss));
   register_packet(SMARTER_MSG_WRITE_ZG_ID, sizeof(smarter_msg_write_zg));

   // NOTE: autopilot
   register_packet(SMARTER_MSG_SEND_REF_ID, sizeof(smarter_msg_send_ref));
   register_packet(SMARTER_MSG_SEND_REF_4DOF_ID, sizeof(smarter_msg_send_ref_4dof));
}

void Smarter_Protocol_CM::connect_to_SAIS(
      const QString& ip, quint16 port,
      quint16 local_port)
{
   if (udp_socket) disconnect_from_SAIS();

   delete udp_socket;
   udp_socket = new QUdpSocket(this);

   QObject::connect(udp_socket, &QUdpSocket::connected,
                    this, [&] ()
   {
      emit socket_msg(QString("[INFO] Socket connected (%1)").arg(udp_socket->state()));
   });

   QObject::connect(udp_socket, &QUdpSocket::disconnected, this, [&]
                    ()
   {
      emit socket_msg(QString("[INFO] Socket disconnected (%1)").arg(udp_socket->state()));
   });

   QObject::connect(udp_socket, &QUdpSocket::errorOccurred, this, [&]
                    (QAbstractSocket::SocketError)
   {
      emit socket_msg(QString("[ERROR] Socket error (%1), SAIS offline?").arg(udp_socket->errorString()));
   });

   QObject::connect(udp_socket, &QUdpSocket::readyRead,
                    this, &Smarter_Protocol_CM::recv_smarter_msg);

   // NOTE: you need to bind before the connectToHost!
   if (!udp_socket->bind(QHostAddress::Any,
                         local_port,
                         QAbstractSocket::ReuseAddressHint))
   {
      emit socket_msg(QString("Cannot bind to port %1 : %2")
                      .arg(local_port)
                      .arg(udp_socket->errorString()));
      return;
   }

   udp_socket->connectToHost(ip, port);
}

void Smarter_Protocol_CM::disconnect_from_SAIS()
{
   if (udp_socket)
   {
      if (udp_socket->state() == QAbstractSocket::UnconnectedState)
      {
         emit socket_msg(QString("[INFO] Socket already disconnected!"));
      }
      else
      {
         udp_socket->disconnectFromHost();
      }
   }
}

void Smarter_Protocol_CM::read_SAIS_status()
{
   smarter_msg_read_status msg_read_status = {};
   msg_read_status.request_code = SMARTER_MSG_READ_STATUS_ID;

   send_smarter_msg(SMARTER_MSG_READ_STATUS_ID, &msg_read_status);
}

void Smarter_Protocol_CM::read_haptic_conf_for_dof_id(DOF_Id dof_id)
{
   smarter_msg_read msg_read = {};
   msg_read.request_code = SMARTER_MSG_READ_ID;
   msg_read.dof = static_cast<SM_uchar>(dof_id);

   // NOTE: The field dof_table is used to specify if the read operation
   //       should return the active configuration (dof_table = 0) or
   //       a configuration stored in the SAIS memory (dof_table > 0)
   //       in position dof_table
   msg_read.dof_table = 0;

   send_smarter_msg(SMARTER_MSG_READ_ID, &msg_read);
}

void Smarter_Protocol_CM::write_SAIS_status(SAIS_Status status)
{
   smarter_msg_write_status msg_write_status = {};
   msg_write_status.timestamp = 0;
   msg_write_status.request_code = SMARTER_MSG_WRITE_STATUS_ID;
   msg_write_status.status = static_cast<SM_uchar>(status);

   send_smarter_msg(SMARTER_MSG_WRITE_STATUS_ID, &msg_write_status);
}

void Smarter_Protocol_CM::write_haptic_conf(const QJsonDocument& json_doc)
{
   // TODO:

   const QString& haptic_conf_type =
       json_doc.object()["haptic_configuration_type"].toString();

   if (haptic_conf_type == "SS")
   {
      auto msg = build_msg_write_ss_from_json_doc(json_doc);

      send_smarter_msg(SMARTER_MSG_WRITE_SS_ID, &msg);
   }
   else if(haptic_conf_type == "ZG")
   {
      auto msg = build_msg_write_zg_from_json_doc(json_doc);

      send_smarter_msg(SMARTER_MSG_WRITE_ZG_ID, &msg);
   }
   else
   {
      emit socket_msg(
               QString("haptic_configuration_type in json is unexpected: '%1'")
               .arg(haptic_conf_type));
   }
}

void Smarter_Protocol_CM::send_smarter_msg(smarter_msg_id msg_id, void* msg)
{
   unsigned char buff[512] = {};
   int byte_encoded = encode(buff, sizeof(buff), msg_id, msg);

   if (byte_encoded < 0)
   {
      emit socket_msg(QString("[ERROR] Cannot encode msg id: %1")
                      .arg(smarter_msg_id_to_str(msg_id)));
      return;
   }

   auto bytes_sent = udp_socket->write(
                        reinterpret_cast<const char*>(buff),
                        static_cast<qint64>(byte_encoded));

   if (bytes_sent < 0)
   {
      emit socket_msg(QString("[ERROR] udp_socket->write failed for msg id: %1 '%2'")
                      .arg(smarter_msg_id_to_str(msg_id))
                      .arg(udp_socket->errorString()));
      return;
   }

}

void Smarter_Protocol_CM::recv_smarter_msg()
{
   while (udp_socket->hasPendingDatagrams())
   {
      QNetworkDatagram datagram = udp_socket->receiveDatagram();

      if (datagram.isNull())
         continue;

      QByteArray data = datagram.data();

      smarter_msg_id id = verify_packet(reinterpret_cast<unsigned char*>(data.data()),
                                        static_cast<int>(data.size()));

      switch (id)
      {

         case SMARTER_INVALID_PACKET:
         {
            emit socket_msg(QString("[WARN] Got invalid packet: <%1> Maybe is not registered?")
                            .arg(data.toHex(' ')));
         } break;

         case SMARTER_MSG_OK_ID:
         {
            smarter_msg_ok msg = {};
            int packet_len = decode(reinterpret_cast<unsigned char*>(data.data()),
                                    static_cast<int>(data.size()),
                                    id, &msg);
            if (packet_len > 0)
            {
               emit msg_SAIS_request_ok(msg);
            }
            else
            {
               // discard packet i guess
            }
         } break;

         case SMARTER_MSG_FAIL_ID:
         {
            smarter_msg_fail msg = {};

            int packet_len = decode(reinterpret_cast<unsigned char*>(data.data()),
                                    static_cast<int>(data.size()),
                                    id, &msg);
            if (packet_len > 0)
            {
               emit msg_SAIS_request_failed(msg);
            }
            else
            {
               // discard packet i guess
            }
         } break;


         case SMARTER_MSG1_STATE_ID:
         {
            smarter_msg1_state msg = {};
            int packet_len = decode(reinterpret_cast<unsigned char*>(data.data()),
                                    static_cast<int>(data.size()),
                                    id, &msg);
            if (packet_len > 0)
            {
               emit msg_SAIS_msg1_state(msg);
            }
            else
            {
               // discard packet i guess
            }
         } break;

         case SMARTER_MSG2_STATE_ID:
         {
            smarter_msg2_state msg = {};
            int packet_len = decode(reinterpret_cast<unsigned char*>(data.data()),
                                    static_cast<int>(data.size()),
                                    id, &msg);
            if (packet_len > 0)
            {
               emit msg_SAIS_msg2_state(msg);
            }
            else
            {
               // discard packet i guess
            }
         } break;

         case SMARTER_MSG3_STATE_ID:
         {
            smarter_msg3_state msg = {};
            int packet_len = decode(reinterpret_cast<unsigned char*>(data.data()),
                                    static_cast<int>(data.size()),
                                    id, &msg);
            if (packet_len > 0)
            {
               emit msg_SAIS_msg3_state(msg);
            }
            else
            {
               // discard packet i guess
            }
         } break;

         case SMARTER_MSG4_STATE_ID:
         {
            smarter_msg4_state msg = {};
            int packet_len = decode(reinterpret_cast<unsigned char*>(data.data()),
                                    static_cast<int>(data.size()),
                                    id, &msg);
            if (packet_len > 0)
            {
               emit msg_SAIS_msg4_state(msg);
            }
            else
            {
               // discard packet i guess
            }
         } break;

         case SMARTER_MSG_STATUS_ID:
         {
            smarter_msg_status msg = {};
            int packet_len = decode(reinterpret_cast<unsigned char*>(data.data()),
                                    static_cast<int>(data.size()),
                                    id, &msg);
            if (packet_len > 0)
            {
               emit msg_SAIS_status(msg);
            }
            else
            {
               // discard packet i guess
            }
         } break;

         case SMARTER_MSG_4DOF_ID:
         {
            smarter_msg_4dof msg = {};
            int packet_len = decode(reinterpret_cast<unsigned char*>(data.data()),
                                    static_cast<int>(data.size()),
                                    id, &msg);
            if (packet_len > 0)
            {
               emit msg_SAIS_4dof(msg);
            }
            else
            {
               // discard packet i guess
            }
         } break;

         case SMARTER_MSG_BUTTONS_ID:
         {
            smarter_msg_buttons msg = {};
            int packet_len = decode(reinterpret_cast<unsigned char*>(data.data()),
                                    static_cast<int>(data.size()),
                                    id, &msg);
            if (packet_len > 0)
            {
               emit msg_SAIS_buttons(msg);
            }
            else
            {
               // discard packet i guess
            }
         } break;

         case SMARTER_MSG_SS_ID:
         {
            smarter_msg_ss msg = {};

            int packet_len = decode(reinterpret_cast<unsigned char*>(data.data()),
                                    static_cast<int>(data.size()),
                                    id, &msg);
            if (packet_len > 0)
            {
               emit msg_SAIS_haptic_conf_ss(msg);
            }
            else
            {
               // discard packet i guess
            }
         } break;

         case SMARTER_MSG_ZG_ID:
         {
            smarter_msg_zg msg = {};

            int packet_len = decode(reinterpret_cast<unsigned char*>(data.data()),
                                    static_cast<int>(data.size()),
                                    id, &msg);
            if (packet_len > 0)
            {
               emit msg_SAIS_haptic_conf_zg(msg);
            }
            else
            {
               // discard packet i guess
            }
         } break;

         default:
            emit socket_msg(QString("[WARN] Got unhandled packet with id: %1 (%2)")
                            .arg(id)
                            .arg(smarter_msg_id_to_str(id)));
         break;
      }

   }

}

