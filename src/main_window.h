#pragma once

#include <QMainWindow>
#include <QUdpSocket>

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


private slots:
   void on_pb_connect_released();
   void on_pb_send_hello_released();

private:
   Ui::Main_Window *ui;
   QUdpSocket* udp_socket = nullptr;

   // QWidget interface
protected:
   virtual void closeEvent(QCloseEvent* event) override;

   // QObject interface
public:
   virtual bool eventFilter(QObject* watched, QEvent* event) override;
};
