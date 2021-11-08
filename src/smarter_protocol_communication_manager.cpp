#include "smarter_protocol_communication_manager.h"

#include <time.h>

#include <QNetworkDatagram>


QString Smarter_Protocol_Communication_Manager::SAIS_Status_to_str(SAIS_Status status)
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


QString Smarter_Protocol_Communication_Manager::smarter_msg_id_to_str(smarter_msg_id msg_id)
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



Smarter_Protocol_Communication_Manager::Smarter_Protocol_Communication_Manager(QObject *parent) : QObject(parent)
{
   //register packets that I want to send or read
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
   register_packet(SMARTER_MSG_SEND_REF_4DOF_ID, sizeof(smarter_msg_send_ref_4dof));

   register_packet(SMARTER_MSG_OK_ID, sizeof(smarter_msg_ok));
   register_packet(SMARTER_MSG_FAIL_ID, sizeof(smarter_msg_fail));

   register_packet(SMARTER_MSG_SETSTOP_SS_ID, sizeof(smarter_msg_setStop_ss));
   register_packet(SMARTER_MSG_SETSTOP_ZG_ID, sizeof(smarter_msg_setStop_zg));

   register_packet(SMARTER_MSG_READ_ID, sizeof(smarter_msg_read));
   register_packet(SMARTER_MSG_SS_ID, sizeof(smarter_msg_ss));
   register_packet(SMARTER_MSG_WRITE_SS_ID, sizeof(smarter_msg_write_ss));

   register_packet(SMARTER_MSG_SEND_REF_ID, sizeof(smarter_msg_send_ref));
}

void Smarter_Protocol_Communication_Manager::connect_to_SAIS(
      const QString& ip, quint16 port,
      quint16 local_port)
{
   if (udp_socket)
      delete udp_socket;

   udp_socket = new QUdpSocket(this);

   QObject::connect(udp_socket, &QUdpSocket::connected, this, [&]
                    ()
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
      emit socket_msg(QString("[ERROR] Socket error (%1)").arg(udp_socket->errorString()));
   });

   QObject::connect(udp_socket, &QUdpSocket::readyRead,
                    this, &Smarter_Protocol_Communication_Manager::recv_smarter_msg);

   // NOTE: you need to bind before the connectToHost!
   if (!udp_socket->bind(QHostAddress::LocalHost,
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

void Smarter_Protocol_Communication_Manager::read_SAIS_status()
{
   smarter_msg_read_status msg_read_status = {};
   msg_read_status.request_code = SMARTER_MSG_READ_STATUS_ID;

   send_smarter_msg(SMARTER_MSG_READ_STATUS_ID, &msg_read_status);
}

void Smarter_Protocol_Communication_Manager::set_SAIS_status(SAIS_Status status)
{
   smarter_msg_write_status msg_write_status = {};
   msg_write_status.timestamp = (clock() / (double)(CLOCKS_PER_SEC)) * 1000000.0;
   msg_write_status.request_code = SMARTER_MSG_WRITE_STATUS_ID;
   msg_write_status.status = (SM_uchar)status;

   send_smarter_msg(SMARTER_MSG_WRITE_STATUS_ID, &msg_write_status);
}



void Smarter_Protocol_Communication_Manager::send_smarter_msg(smarter_msg_id msg_id, void* msg)
{
   unsigned char buff[512] = {};
   int byte_encoded = encode(buff, sizeof(buff), msg_id, msg);

   if (byte_encoded < 0)
   {
      emit socket_msg(QString("[ERROR] Cannot encode msg id %1").arg(smarter_msg_id_to_str(msg_id)));
      return;
   }

   auto bytes_sent = udp_socket->write(
                        reinterpret_cast<const char*>(buff),
                        static_cast<qint64>(byte_encoded));

   if (bytes_sent < 0)
   {
      emit socket_msg(QString("[ERROR] udp_socket->write failed for msg id %1: '%2'")
                      .arg(smarter_msg_id_to_str(msg_id))
                      .arg(udp_socket->errorString()));
      return;
   }

}

void Smarter_Protocol_Communication_Manager::recv_smarter_msg()
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

         case SMARTER_MSG_STATUS_ID:
         {
            smarter_msg_status msg = {};
            int packet_len = decode(reinterpret_cast<unsigned char*>(data.data()),
                                    static_cast<int>(data.size()),
                                    id, &msg);
            if (packet_len > 0)
            {
               emit mag_SAIS_status(msg);
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

         case SMARTER_INVALID_PACKET:
         {
            // TODO: do something?
             emit socket_msg(QString("[WARN] Got invalid packet: <%1>").arg(data.toHex(' ')));
         } break;

         default:
         break;
      }


   }


}

