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
//   showMaximized();

   ui->pb_refresh_ips->click();

   ui->axis_1->set_axis_name("Axis 0 (ROLL)");
   ui->axis_1->set_dof_id(DOF_Id::ROLL);
   ui->axis_1->set_dof_type(DOF_Type::ROTATIONAL);

   ui->axis_2->set_axis_name("Axis 1 (PITCH)");
   ui->axis_2->set_dof_id(DOF_Id::PITCH);
   ui->axis_2->set_dof_type(DOF_Type::ROTATIONAL);

   ui->axis_3->set_axis_name("Axis 2 (YAW)");
   ui->axis_3->set_dof_id(DOF_Id::YAW);
   ui->axis_3->set_dof_type(DOF_Type::ROTATIONAL);
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

   delete smarter_protocol_cm;
   smarter_protocol_cm = new Smarter_Protocol_CM(this);

   connect(smarter_protocol_cm, &SmarterPCM::socket_msg,
           this, &Main_Window::add_log_msg);

   connect(smarter_protocol_cm, &SmarterPCM::msg_SAIS_status,
           this, [&]
           (smarter_msg_status msg)
   {
      //float at_ms = static_cast<float>(msg.timestamp) / 1000.0f;
      add_log_msg(QString("SAIS status: %1")
                  .arg(to_str(static_cast<SAIS_Status>(msg.status))));
   });

   connect(smarter_protocol_cm, &SmarterPCM::msg_SAIS_request_ok,
           this, [&]
           (smarter_msg_ok msg_ok)
   {
      QString msg = QString("OK for request: %1")
                    .arg(smarter_msg_id_to_str(msg_ok.request_code));
      add_log_msg(msg);
   });

   connect(smarter_protocol_cm, &SmarterPCM::msg_SAIS_request_failed,
           this, [&]
           (smarter_msg_fail msg_fail)
   {
      QString msg = QString("FAIL for request: %1 SAIS say: '%2' error code: %3")
                    .arg(smarter_msg_id_to_str(msg_fail.request_code))
                    .arg(reinterpret_cast<char*>(msg_fail.error_string))
                    .arg(msg_fail.error_code);
      add_log_msg(msg);
   });


   connect(smarter_protocol_cm, &SmarterPCM::msg_SAIS_4dof,
           ui->axis_1, &Axis_Widget::refresh_4dof);
   connect(smarter_protocol_cm, &SmarterPCM::msg_SAIS_4dof,
           ui->axis_2, &Axis_Widget::refresh_4dof);
   connect(smarter_protocol_cm, &SmarterPCM::msg_SAIS_4dof,
           ui->axis_3, &Axis_Widget::refresh_4dof);

// TODO: aggiungere per tutti gli altri messaggi
   connect(smarter_protocol_cm, &SmarterPCM::msg_SAIS_msg1_state,
           this, [&](smarter_msg1_state msg)
   {
      ui->axis_1->refresh_dof(msg.pvf);
   });

   connect(smarter_protocol_cm, &SmarterPCM::msg_SAIS_haptic_conf_ss,
           this, [&] (smarter_msg_ss msg_ss)
   {
      QJsonDocument json = to_json(msg_ss);

      ui->te_haptic_conf->setPlainText(
               QString::fromUtf8(json.toJson()));

      QString dof_id_str =
            to_str(static_cast<DOF_Id>(json.object()["dof"].toInt()));
      QString dof_type_str =
            to_str(static_cast<DOF_Type>(json.object()["ss_table"].toObject()["dof_type"].toInt()));
      QString hc_type_str =
            json.object()["haptic_configuration_type"].toString();

      update_labels_haptic_config(dof_id_str,
                                  dof_type_str,
                                  hc_type_str);

      switch(static_cast<DOF_Id>(json.object()["dof"].toInt()))
      {
         case DOF_Id::ROLL:
            ui->pb_write_config_dof_1->setEnabled(true);
         break;

         case DOF_Id::PITCH:
            ui->pb_write_config_dof_2->setEnabled(true);
         break;

         case DOF_Id::YAW:
            ui->pb_write_config_dof_3->setEnabled(true);
         break;

         default:
         break;
      }

   });

   connect(smarter_protocol_cm, &SmarterPCM::msg_SAIS_haptic_conf_zg,
           this, [&] (smarter_msg_zg msg_zg)
   {
      QJsonDocument json = to_json(msg_zg);

      ui->te_haptic_conf->setPlainText(
               QString::fromUtf8(json.toJson()));

      QString dof_id_str =
            to_str(static_cast<DOF_Id>(json.object()["dof"].toInt()));
      QString dof_type_str =
            to_str(static_cast<DOF_Type>(json.object()["zg_table"].toObject()["dof_type"].toInt()));
      QString hc_type_str =
            json.object()["haptic_configuration_type"].toString();

      update_labels_haptic_config(dof_id_str,
                                  dof_type_str,
                                  hc_type_str);

      switch(static_cast<DOF_Id>(json.object()["dof"].toInt()))
      {
         case DOF_Id::ROLL:
            ui->pb_write_config_dof_1->setEnabled(true);
         break;

         case DOF_Id::PITCH:
            ui->pb_write_config_dof_2->setEnabled(true);
         break;

         case DOF_Id::YAW:
            ui->pb_write_config_dof_3->setEnabled(true);
         break;

         default:
         break;
      }
   });

   connect(smarter_protocol_cm, &SmarterPCM::msg_SAIS_4dof,
           this, [&](smarter_msg_4dof msg)
   {
      //qInfo() << "Axis1:" << msg.axis1 <<
      //           "Axis2:" << msg.axis2 <<
      //           "Axis3:" << msg.axis3 <<
      //           "Axis4:" << msg.axis4;

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

   smarter_protocol_cm->connect_to_SAIS(ui->le_joystick_ip->text(),
                         ui->le_joystick_port->text().toUShort(),
                         ui->le_local_port->text().toUShort());
}

void Main_Window::on_pb_disconnect_clicked()
{
    if (smarter_protocol_cm)
       smarter_protocol_cm->disconnect_from_SAIS();
}



void Main_Window::on_pb_go_active_clicked()
{
   if (smarter_protocol_cm)
      smarter_protocol_cm->write_SAIS_status(SAIS_Status::ACTIVE);
   else
      add_log_msg("[ERROR] Not connected!");
}

void Main_Window::on_pb_go_passive_clicked()
{
   if (smarter_protocol_cm)
      smarter_protocol_cm->write_SAIS_status(SAIS_Status::PASSIVE);
   else
      add_log_msg("[ERROR] Not connected!");
}

void Main_Window::on_pb_go_standby_clicked()
{
   if (smarter_protocol_cm)
      smarter_protocol_cm->write_SAIS_status(SAIS_Status::STANDBY);
   else
      add_log_msg("[ERROR] Not connected!");
}

void Main_Window::on_pb_read_status_clicked()
{
   if (smarter_protocol_cm)
      smarter_protocol_cm->read_SAIS_status();
   else
      add_log_msg("[ERROR] Not connected!");
}



void Main_Window::on_pb_read_config_dof_1_clicked()
{
   if (smarter_protocol_cm)
   {
      ui->pb_clear_haptic_conf->click();
      smarter_protocol_cm->read_haptic_conf_for_dof_id(DOF_Id::ROLL);
   }
   else add_log_msg("[ERROR] Not connected!");
}

void Main_Window::on_pb_read_config_dof_2_clicked()
{
   if (smarter_protocol_cm)
   {
      smarter_protocol_cm->read_haptic_conf_for_dof_id(DOF_Id::PITCH);
      ui->pb_clear_haptic_conf->click();
   }
   else add_log_msg("[ERROR] Not connected!");
}

void Main_Window::on_pb_read_config_dof_3_clicked()
{
   if (smarter_protocol_cm)
   {
      smarter_protocol_cm->read_haptic_conf_for_dof_id(DOF_Id::YAW);
      ui->pb_clear_haptic_conf->click();
   }
   else add_log_msg("[ERROR] Not connected!");
}

void Main_Window::update_labels_haptic_config(const QString& dof_id,
                                              const QString& dof_type,
                                              const QString& hc_type)
{
   ui->lbl_hc_dof_id->setText(QString("DOF: %1")
                              .arg(dof_id));
   ui->lbl_hc_dof_type->setText(QString("DOF type: %1")
                                .arg(dof_type));
   ui->lbl_hc_hc_type->setText(QString("Haptic Config Type: %1")
                               .arg(hc_type));
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


void Main_Window::on_pb_refresh_ips_clicked()
{
   QStringList all_ips;

   QList<QHostAddress> all_address = QNetworkInterface::allAddresses();
   for (const QHostAddress& host_addr : all_address)
   {
      if (host_addr.protocol() == QAbstractSocket::IPv4Protocol &&
          host_addr != QHostAddress::LocalHost)
      {
         all_ips << host_addr.toString();
      }
   }
   ui->le_ips->setText(all_ips.join(","));
}

void Main_Window::on_pb_clear_haptic_conf_clicked()
{
    update_labels_haptic_config("", "", "");
    ui->pb_write_config_dof_1->setEnabled(false);
    ui->pb_write_config_dof_2->setEnabled(false);
    ui->pb_write_config_dof_3->setEnabled(false);
}


void Main_Window::on_pb_write_config_dof_1_clicked()
{
   prepare_msg_for_write_haptic_configuration();
}


void Main_Window::on_pb_write_config_dof_2_clicked()
{
   prepare_msg_for_write_haptic_configuration();
}


void Main_Window::on_pb_write_config_dof_3_clicked()
{
   prepare_msg_for_write_haptic_configuration();
}

void Main_Window::prepare_msg_for_write_haptic_configuration()
{
   // read text
   // conver to json and check that all the fiels are present
   // based on haptic_configuration_type send message
   //   smarter_msg_write_ss or smarter_msg_write_zg

   QString haptic_cons_str = ui->te_haptic_conf->toPlainText();

   QJsonParseError error;
   QJsonDocument json_doc = QJsonDocument::fromJson(
                               haptic_cons_str.toLatin1(),
                               &error);

   if (json_doc.isNull())
   {
      QMessageBox::warning(this,
                           windowTitle(),
                           QString("Haptic Configuration is malformed!\n"
                                   "'%1' at offset: %2")
                           .arg(error.errorString())
                           .arg(error.offset)
                           );
      // TODO: highlight line where error occurred

      //auto text_cursor = ui->te_haptic_conf->textCursor();
      //qInfo() << "text_cursor.position" << text_cursor.position();
      //qInfo() << "text_cursor.positionInBlock" << text_cursor.positionInBlock();
      //
      //text_cursor.setPosition(error.offset);
      //ui->te_haptic_conf->setTextCursor(text_cursor);

      return;
   }


   if (const auto& res = verify_haptic_config_json(json_doc);
       !res.valid)
   {
      QMessageBox::warning(this,
                           windowTitle(),
                           QString("Haptic Configuration is invalid!\n%1")
                           .arg(res.error));

      return;
   }

   // TODO: re enable this
   // smarter_protocol_cm->write_haptic_conf(json_doc);
}

