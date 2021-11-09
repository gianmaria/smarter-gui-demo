#pragma once

#include <QWidget>

namespace Ui {
class Axis_Widget;
}

extern "C" {
#include "smarter_protocol_streaming.h"
}

class Axis_Widget : public QWidget
{
   Q_OBJECT

public:

   // 0:linear, 1: rotational 16 bit
   enum DOF_Type : SM_uchar
   {
      LINEAR = 0,
      ROTATIONAL = 1,
      UNKNOWN
   };

   static QString DOF_Type_to_str(DOF_Type type);


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

   void set_dof_id(int id);
   void set_dof_type(Axis_Widget::DOF_Type type);

   void set_axis_pos(int pos);
   void set_axis_vel(int vel);
   void set_axis_force(int force);

   void update_4dof(smarter_msg_4dof msg);

private slots:
   void on_axis_pos_valueChanged(int value);

   void on_axis_velocity_valueChanged(int value);

   void on_axis_force_valueChanged(int value);

private:
   Ui::Axis_Widget *ui;
   int dof_id = -1;
   DOF_Type dof_type = DOF_Type::UNKNOWN;

   const float SCALING_FACTOR_ANGULAR_POS = 100.0f;
   const float SCALING_FACTOR_ANGULAR_VEL = 100.0f;
   const float SCALING_FACTOR_FORCE = 100.0f;
   const float SCALING_FACTOR_LINEAR_POS = 50.0f;
   const float SCALING_FACTOR_LINEAR_VEL = 50.0f;
};

