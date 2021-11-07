#include "main_window.h"
#include "ui_main_window.h"

#include <QCloseEvent>
#include <QDateTime>
#include <QHostAddress>
#include <QKeyEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QNetworkInterface>
#include <QSettings>

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

void Main_Window::on_pb_connect_released()
{
   write_settings();

   if (udp_socket)
      delete udp_socket;

   udp_socket = new QUdpSocket(this);

   QObject::connect(udp_socket, &QUdpSocket::connected, this, [&]
                    ()
   {
      QString msg = "Connected!";
      add_log_msg(msg);
      ui->statusbar->showMessage(msg);
   });

   QObject::connect(udp_socket, &QUdpSocket::errorOccurred, this, [&]
                    (QAbstractSocket::SocketError socket_error)
   {
      //qCritical() << socket_error << udp_socket->errorString();

      QString msg = "Cannot connect to <" + ui->le_joystick_ip->text() + ":" + ui->le_joystick_port->text() + ">\n" +
                        udp_socket->errorString();
      QMessageBox::critical(this, windowTitle(), msg);

      ui->statusbar->showMessage(msg);

      add_log_msg(msg);
   });

   QObject::connect(udp_socket, &QUdpSocket::disconnected, this, [&]
                    ()
   {
      QString msg = "Socket disconnected";
      ui->statusbar->showMessage(msg);
      add_log_msg(msg);
   });

   // NOTE: you need to bind before the connectToHost!
   if (!udp_socket->bind(QHostAddress::LocalHost, ui->le_local_port->text().toUShort(), QAbstractSocket::ReuseAddressHint))
   {
      QString msg = "Cannot bind to port <" + ui->le_local_port->text() + ">\n" +
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
      qInfo() << "Socket state:" << udp_socket->state();
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

void Main_Window::closeEvent(QCloseEvent* event)
{
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
