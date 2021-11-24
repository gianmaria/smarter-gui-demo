#pragma once

#include <QMainWindow>

#include "smarter_protocol_cm.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Main_Window; }
QT_END_NAMESPACE

class Main_Window : public QMainWindow
{
   Q_OBJECT

public:
   Main_Window(QWidget *parent = nullptr);
   virtual bool eventFilter(QObject* watched, QEvent* event) override;
   ~Main_Window();

   void read_settings();
   void write_settings();

public slots:
   void add_log_msg(const QString& msg);

protected:
   virtual void closeEvent(QCloseEvent* event) override;


private slots:
   void on_pb_connect_clicked();
   void on_pb_disconnect_clicked();

   void on_pb_go_active_clicked();
   void on_pb_go_passive_clicked();
   void on_pb_go_standby_clicked();
   void on_pb_read_status_clicked();

   void on_pb_read_config_dof_0_clicked();
   void on_pb_read_config_dof_1_clicked();
   void on_pb_read_config_dof_2_clicked();

   void update_labels_haptic_config(const QString& dof_id,
                                    const QString& dof_type,
                                    const QString& hc_type);

   void on_pb_refresh_ips_clicked();

   void on_pb_clear_haptic_conf_clicked();

   void on_pb_write_config_dof_0_clicked();
   void on_pb_write_config_dof_1_clicked();
   void on_pb_write_config_dof_2_clicked();

   void prepare_msg_for_write_haptic_configuration();

   void on_pb_send_rt_haptic_pos_clicked();

   void on_pb_reset_haptic_pos_clicked();

private:
   Ui::Main_Window *ui;
   Smarter_Protocol_CM* smarter_protocol_cm = nullptr;
};
