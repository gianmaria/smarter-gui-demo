#pragma once

#include <QWidget>

namespace Ui {
class Axis_Widget;
}

class Axis_Widget : public QWidget
{
   Q_OBJECT

public:
   explicit Axis_Widget(QWidget *parent = nullptr);
   ~Axis_Widget();

public slots:

   void set_axis_pos_min(int value);
   void set_axis_pos_max(int value);

   void set_axis_force_min(int value);
   void set_axis_force_max(int value);

   void set_axis_vel_min(int value);
   void set_axis_vel_max(int value);

   void set_axis_name(const QString& name);
   void set_axis_type(const QString& type);
   void set_axis_pos(int pos);
   void set_axis_vel(int vel);
   void set_axis_force(int force);

private slots:
   void on_axis_pos_valueChanged(int value);

private:
   Ui::Axis_Widget *ui;
};

