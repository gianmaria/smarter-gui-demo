#include "main_window.h"
#include "ui_main_window.h"

#include <QCloseEvent>
#include <QDateTime>
#include <QHostAddress>
#include <QKeyEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QNetworkDatagram>
#include <QNetworkInterface>
#include <QSettings>

extern "C" {
#include "smarter_protocol_streaming.h"
}

Main_Window::Main_Window(QWidget *parent)
   : QMainWindow(parent)
   , ui(new Ui::Main_Window)
{
   ui->setupUi(this);
   installEventFilter(this);

   //qInfo() << this << Q_FUNC_INFO;

   read_settings();

   QList<QHostAddress> all_address = QNetworkInterface::allAddresses();
   for (const QHostAddress& host_addr : all_address)
   {
      if (host_addr.protocol() == QAbstractSocket::IPv4Protocol &&
          host_addr != QHostAddress::LocalHost)
      {
         //qInfo() << host_addr.toString();
         ui->le_ips->insert(host_addr.toString());
         ui->le_ips->insert(", ");
      }
   }
   ui->le_ips->setCursorPosition(0);

   ui->axis_1->set_axis_name("Axis 1");
   ui->axis_2->set_axis_name("Axis 2");
   ui->axis_3->set_axis_name("Axis 3");

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

Main_Window::~Main_Window()
{
   delete ui;
}

void Main_Window::read_settings()
{
   QSettings settings(windowTitle().toLower().replace(" ", "_").replace("-", "_") + ".ini", QSettings::IniFormat, this);

   settings.beginGroup("MainWindow");
   resize(settings.value("size", QSize(400, 400)).toSize());
   move(settings.value("pos", QPoint(200, 200)).toPoint());
   settings.endGroup();

   settings.beginGroup("Joystick");
   ui->le_joystick_ip->setText(settings.value("joystick_ip", "").toString());
   ui->le_joystick_port->setText(settings.value("joystick_port", "").toString());
   ui->le_local_port->setText(settings.value("local_port", "").toString());
   settings.endGroup();
}

void Main_Window::write_settings()
{
   QSettings settings(windowTitle().toLower().replace(" ", "_").replace("-", "_") + ".ini", QSettings::IniFormat, this);

   settings.beginGroup("MainWindow");
   settings.setValue("size", size());
   settings.setValue("pos", pos());
   settings.endGroup();

   settings.beginGroup("Joystick");
   settings.setValue("joystick_ip", ui->le_joystick_ip->text());
   settings.setValue("joystick_port", ui->le_joystick_port->text());
   settings.setValue("local_port", ui->le_local_port->text());
   settings.endGroup();
}

void Main_Window::add_log_msg(const QString& msg)
{
   auto now = QDateTime::currentDateTime();
   QString text = QString("%1 - %2").arg(now.toString("hh:mm:ss")).arg(msg);
   ui->te_log->appendPlainText(text);
}

void Main_Window::send_smarter_msg(smarter_msg_id msg_id, void* msg)
{
   unsigned char buff[512] = {};
   int byte_encoded = encode(buff, sizeof(buff), msg_id, msg);

   if (byte_encoded < 0)
   {
      add_log_msg(QString("[ERROR] Cannot encode msg id %1").arg(msg_id_to_str(msg_id)));
      return;
   }

   auto bytes_sent = udp_socket->write(
                        reinterpret_cast<const char*>(buff),
                        static_cast<qint64>(byte_encoded));

   if (bytes_sent < 0)
   {
      add_log_msg(QString("[ERROR] Sending Data failed for msg id %1: %2")
                  .arg(msg_id_to_str(msg_id))
                  .arg(udp_socket->errorString())
                  );
      return;
   }

}

void Main_Window::recv_smarter_msg()
{
   while (udp_socket->hasPendingDatagrams())
   {
      QNetworkDatagram datagram = udp_socket->receiveDatagram();

      if (datagram.isNull())
         continue;

      QByteArray data = datagram.data();

      auto msg = QString("[INFO] Got msg from %1 from port %2: '%3'")
                 .arg(datagram.senderAddress().toString())
                 .arg(datagram.senderPort())
                 .arg(data.toHex(' '));
      add_log_msg(msg);

      smarter_msg_id id = verify_packet(reinterpret_cast<unsigned char*>(data.data()),
                                        static_cast<int>(data.size()));

      switch (id)
      {
         case SMARTER_MSG_STATUS_ID:
         {
            smarter_msg_status msg_status;
            int packet_len = decode(reinterpret_cast<unsigned char*>(data.data()),
                                    static_cast<int>(data.size()),
                                    id, &msg_status);
            if (packet_len > 0)
            {
               auto msg = QString("%1ms Status: %2")
                          .arg(static_cast<float>(msg_status.timestamp) / 1000.0f)
                          .arg(smarter_status_to_str(msg_status.status));
               add_log_msg(msg);
            }
            else
            {
               add_log_msg("Error!!!!");
            }
         } break;

         case SMARTER_INVALID_PACKET:
         break;

         default:
         break;
      }

   }

}

void Main_Window::on_pb_connect_released()
{
   write_settings();

   udp_socket.clear();
   udp_socket = QSharedPointer<QUdpSocket>(new QUdpSocket(this));

   QObject::connect(udp_socket.data(), &QUdpSocket::connected, this, [&]
                    ()
   {
      if (udp_socket->state() != QAbstractSocket::ConnectedState)
         return ;

      QString msg = "Connected!";
      add_log_msg(msg);
      ui->statusbar->showMessage(msg);

      smarter_msg_read_status msg_read_status = {};
      msg_read_status.request_code = 200;

      send_smarter_msg(SMARTER_MSG_READ_STATUS_ID, &msg_read_status);

   });

   QObject::connect(udp_socket.data(), &QUdpSocket::errorOccurred, this, [&]
                    (QAbstractSocket::SocketError)
   {
      //qCritical() << socket_error << udp_socket->errorString();

      QString msg = "Cannot connect to <" + ui->le_joystick_ip->text() + " : " + ui->le_joystick_port->text() + ">\n" +
                        udp_socket->errorString();
      QMessageBox::critical(this, windowTitle(), msg);

      ui->statusbar->showMessage(msg);

      add_log_msg(msg);
   });

   QObject::connect(udp_socket.data(), &QUdpSocket::disconnected, this, [&]
                    ()
   {
      QString msg = "Socket disconnected";
      ui->statusbar->showMessage(msg);
      add_log_msg(msg);
   });


   QObject::connect(udp_socket.data(), &QUdpSocket::readyRead,
           this, &Main_Window::recv_smarter_msg);

   // NOTE: you need to bind before the connectToHost!
   if (!udp_socket->bind(QHostAddress::LocalHost,
                         ui->le_local_port->text().toUShort(),
                         QAbstractSocket::ReuseAddressHint))
   {
      QString msg = "Cannot bind to port <" +
                    ui->le_local_port->text() + ">\n" +
                    udp_socket->errorString();

      QMessageBox::critical(this,
                            windowTitle(),
                            msg);
      add_log_msg(msg);

      return;
   }

   udp_socket->connectToHost(ui->le_joystick_ip->text(),
                             ui->le_joystick_port->text().toUShort());
}

void Main_Window::on_pb_send_hello_released()
{
   if (udp_socket &&
       udp_socket->state() == QAbstractSocket::ConnectedState)
   {
      static int counter = 1;
      QByteArray data("Hello friend: ");
      data.append(QString::number(counter++).toUtf8());

      udp_socket->write(data);
   }
   else
   {
      QString message = "Socket invalid: " + QVariant::fromValue(udp_socket->state()).toString();
      qInfo() << message;
      ui->statusbar->showMessage(message);
   }
}

QString Main_Window::msg_id_to_str(smarter_msg_id msg_id) const
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

void Main_Window::closeEvent(QCloseEvent* event)
{
   qDebug() << Q_FUNC_INFO;

   write_settings();
   event->accept();
}

bool Main_Window::eventFilter(QObject* watched, QEvent* event)
{
   if (watched == this)
   {
      if (event->type() == QEvent::KeyPress)
      {
         QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
         if (keyEvent->matches(QKeySequence::Cancel))
         {
            qDebug() << Q_FUNC_INFO;

            auto ret = QMessageBox::question(this, windowTitle(), "Closing?");
            switch (ret) {
              case QMessageBox::Yes:
                  qApp->quit();
                  return true;
                  break;
              case QMessageBox::No:
                  return false;
                  break;
              default:
                  // should never be reached
                  return false;
                  break;
            }
         }
         else
         {
            return false;
         }
      }
      else
      {
         return false;
      }
   }
   else
   {
      // pass the event on to the parent class
      return QMainWindow::eventFilter(watched, event);
   }
}
