#include "main_window.h"
#include "ui_main_window.h"

#include <QCloseEvent>
#include <QDateTime>
#include <QHostAddress>
#include <QJsonObject>
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

   read_settings();

   QList<QHostAddress> all_address = QNetworkInterface::allAddresses();
   for (const QHostAddress& host_addr : all_address)
   {
      if (host_addr.protocol() == QAbstractSocket::IPv4Protocol &&
          host_addr != QHostAddress::LocalHost)
      {
         ui->le_ips->insert(host_addr.toString());
         ui->le_ips->insert(", ");
      }
   }
   ui->le_ips->setCursorPosition(0);

   ui->axis_1->set_axis_name("Axis ROLL (0)");
   ui->axis_1->set_dof_id(DOF_Id::ROLL);
   ui->axis_1->set_dof_type(DOF_Type::ROTATIONAL);
   ui->axis_1->set_axis_pos_min(-17);
   ui->axis_1->set_axis_pos_max(17);
   ui->axis_1->set_axis_vel_min(0);
   ui->axis_1->set_axis_vel_max(60);
   ui->axis_1->set_axis_force_min(0);
   ui->axis_1->set_axis_force_max(240);

   ui->axis_2->set_axis_name("Axis PITCH (1)");
   ui->axis_2->set_dof_id(DOF_Id::PITCH);
   ui->axis_2->set_dof_type(DOF_Type::ROTATIONAL);
   ui->axis_2->set_axis_pos_min(-17);
   ui->axis_2->set_axis_pos_max(17);
   ui->axis_2->set_axis_vel_min(0);
   ui->axis_2->set_axis_vel_max(60);
   ui->axis_2->set_axis_force_min(0);
   ui->axis_2->set_axis_force_max(240);

   ui->axis_3->set_axis_name("Axis YAW (2)");
   ui->axis_3->set_dof_id(DOF_Id::YAW);
   ui->axis_3->set_dof_type(DOF_Type::ROTATIONAL);
   ui->axis_3->set_axis_pos_min(-17);
   ui->axis_3->set_axis_pos_max(17);
   ui->axis_3->set_axis_vel_min(0);
   ui->axis_3->set_axis_vel_max(60);
   ui->axis_3->set_axis_force_min(0);
   ui->axis_3->set_axis_force_max(240);

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
   QString text = QString("%1 - %2").arg(now.toString("hh:mm:ss"), msg);
   ui->te_log->appendPlainText(text);

   ui->statusbar->showMessage(msg, 5000);
}



void Main_Window::on_pb_connect_clicked()
{
   write_settings();

   delete spcm;
   spcm = new Smarter_Protocol_CM(this);

   connect(spcm, &SmarterPCM::socket_msg,
           this, &Main_Window::add_log_msg);

   connect(spcm, &SmarterPCM::mag_SAIS_status,
           this, [&]
           (smarter_msg_status msg)
   {
      //float at_ms = static_cast<float>(msg.timestamp) / 1000.0f;
      add_log_msg(QString("SAIS status: %1")
                  .arg(to_str(static_cast<SAIS_Status>(msg.status))));
   });

   connect(spcm, &SmarterPCM::msg_SAIS_request_ok,
           this, [&]
           (smarter_msg_ok msg_ok)
   {
      QString msg = QString("OK for request: %1")
                    .arg(smarter_msg_id_to_str(msg_ok.request_code));
      add_log_msg(msg);
   });

   connect(spcm, &SmarterPCM::msg_SAIS_request_failed,
           this, [&]
           (smarter_msg_fail msg_fail)
   {
      QString msg = QString("FAIL for request: %1 SAIS say: '%2' error code: %3")
                    .arg(smarter_msg_id_to_str(msg_fail.request_code))
                    .arg(reinterpret_cast<char*>(msg_fail.error_string))
                    .arg(msg_fail.error_code);
      add_log_msg(msg);
   });


   connect(spcm, &SmarterPCM::msg_SAIS_4dof,
           ui->axis_1, &Axis_Widget::refresh_4dof);
   connect(spcm, &SmarterPCM::msg_SAIS_4dof,
           ui->axis_2, &Axis_Widget::refresh_4dof);
   connect(spcm, &SmarterPCM::msg_SAIS_4dof,
           ui->axis_3, &Axis_Widget::refresh_4dof);


   connect(spcm, &SmarterPCM::msg_SAIS_haptic_conf_ss,
           this, [&] (smarter_msg_ss msg_ss)
   {
      QJsonDocument json = to_json(msg_ss);

      ui->te_haptic_conf->setPlainText(
               QString::fromUtf8(json.toJson()));
   });

   connect(spcm, &SmarterPCM::msg_SAIS_haptic_conf_zg,
           this, [&] (smarter_msg_zg msg_zg)
   {
      QJsonDocument json = to_json(msg_zg);

      ui->te_haptic_conf->setPlainText(
               QString::fromUtf8(json.toJson()));
   });

   connect(spcm, &SmarterPCM::msg_SAIS_4dof,
           this, [&](smarter_msg_4dof msg)
   {
      QList<QPushButton*> all_buttons = ui->tab_widget->findChildren<QPushButton*>();

      for (qsizetype i = 0;
           i < all_buttons.size();
           ++i)
      {
         QPushButton* b = all_buttons[i];

         for (unsigned j = 0;
              j < 16;
              ++j)
         {
            if (b->objectName() == QString("pb_b%1").arg(j))
            {
               if (msg.buttons_state & (static_cast<unsigned>(0x1) << j))
                  b->setChecked(true);
               else
                  b->setChecked(false);
            }
         }
      }
   });

   spcm->connect_to_SAIS(ui->le_joystick_ip->text(),
                         ui->le_joystick_port->text().toUShort(),
                         ui->le_local_port->text().toUShort());
}

void Main_Window::on_pb_go_active_clicked()
{
   if (spcm)
      spcm->set_SAIS_status(SAIS_Status::ACTIVE);
   else
      add_log_msg("[ERROR] Not connected!");
}

void Main_Window::on_pb_read_status_clicked()
{
   if (spcm)
      spcm->read_SAIS_status();
   else
      add_log_msg("[ERROR] Not connected!");
}

void Main_Window::on_pb_go_standby_clicked()
{
   if (spcm)
      spcm->set_SAIS_status(SAIS_Status::STANDBY);
   else
      add_log_msg("[ERROR] Not connected!");
}

void Main_Window::on_pb_read_config_dof_1_clicked()
{
   if (spcm) spcm->read_haptic_conf_for_dof_id(0);
   else add_log_msg("[ERROR] Not connected!");
}

void Main_Window::on_pb_read_config_dof_2_clicked()
{
   if (spcm) spcm->read_haptic_conf_for_dof_id(1);
   else add_log_msg("[ERROR] Not connected!");
}

void Main_Window::on_pb_read_config_dof_3_clicked()
{
   if (spcm) spcm->read_haptic_conf_for_dof_id(2);
   else add_log_msg("[ERROR] Not connected!");
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


