#pragma once

#include <QMainWindow>
#include <QUdpSocket>

extern "C"
{
#include "smarter_protocol_streaming.h"
}

QT_BEGIN_NAMESPACE
namespace Ui { class Main_Window; }
QT_END_NAMESPACE

class Main_Window : public QMainWindow
{
   Q_OBJECT

public:
   Main_Window(QWidget *parent = nullptr);
   ~Main_Window();

   void read_settings();
   void write_settings();

public slots:
   void add_log_msg(const QString& msg);
   void send_smarter_msg(smarter_msg_id msg_id, void* msg);
   void recv_smarter_msg();

private slots:
   void on_pb_connect_released();
   void on_pb_send_hello_released();

private:
   Ui::Main_Window *ui;
   QSharedPointer<QUdpSocket> udp_socket;

   QString msg_id_to_str(smarter_msg_id msg_id) const;

   // QWidget interface
protected:
   virtual void closeEvent(QCloseEvent* event) override;

   // QObject interface
public:
   virtual bool eventFilter(QObject* watched, QEvent* event) override;
};
