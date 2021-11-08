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
   ui->axis_1->set_axis_pos_min(-1700);
   ui->axis_1->set_axis_pos_max(1700);
   ui->axis_1->set_axis_force_min(0);
   ui->axis_1->set_axis_force_max(30);
   ui->axis_1->set_axis_vel_min(0);
   ui->axis_1->set_axis_vel_max(30);

   ui->axis_2->set_axis_name("Axis 2");
   ui->axis_2->set_axis_pos_min(-1700);
   ui->axis_2->set_axis_pos_max(1700);
   ui->axis_2->set_axis_force_min(0);
   ui->axis_2->set_axis_force_max(30);
   ui->axis_2->set_axis_vel_min(0);
   ui->axis_2->set_axis_vel_max(30);

   ui->axis_3->set_axis_name("Axis 3");
   ui->axis_3->set_axis_pos_min(-1700);
   ui->axis_3->set_axis_pos_max(1700);
   ui->axis_3->set_axis_force_min(0);
   ui->axis_3->set_axis_force_max(30);
   ui->axis_3->set_axis_vel_min(0);
   ui->axis_3->set_axis_vel_max(30);

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

void Main_Window::add_log_msg(QString msg)
{
   auto now = QDateTime::currentDateTime();
   QString text = QString("%1 - %2").arg(now.toString("hh:mm:ss")).arg(msg);
   ui->te_log->appendPlainText(text);
}

void Main_Window::on_pb_connect_released()
{
   write_settings();

   if (spcm)
      delete spcm;

   spcm = new Smarter_Protocol_Communication_Manager(this);

   connect(spcm, &Smarter_Protocol_Communication_Manager::socket_msg,
           this, &Main_Window::add_log_msg);

   connect(spcm, &Smarter_Protocol_Communication_Manager::SAIS_status,
           this, [&]
           (smarter_msg_status msg)
   {
      float at_ms = static_cast<float>(msg.timestamp) / 1000.0f;
      add_log_msg(QString("%1ms - %2")
                  .arg(at_ms)
                  .arg(spcm->status_to_str(msg.status)));
   });


   connect(spcm, &Smarter_Protocol_Communication_Manager::SAIS_4dof,
           this, [&]
           (smarter_msg_4dof msg)
   {
      ui->axis_1->set_axis_type(msg.pvf[0].dof_type == 0 ? "Linear" : "Rotational"); // 0:linear, 1: rotational 16 bit
      ui->axis_1->set_axis_pos(msg.pvf[0].position);
      ui->axis_1->set_axis_force(msg.pvf[0].force);
      ui->axis_1->set_axis_vel(msg.pvf[0].velocity);
   });

   spcm->connect_to_SAIS(ui->le_joystick_ip->text(),
                         ui->le_joystick_port->text().toUShort(),
                         ui->le_local_port->text().toUShort());
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

void Main_Window::on_pb_go_active_released()
{
   spcm->set_SAIS_status(2);
   spcm->request_SAIS_status();
}

