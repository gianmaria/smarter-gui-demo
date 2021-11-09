#pragma once

#include <QWidget>

#include "SAIS_common.h"

extern "C" {
#include "smarter_protocol_streaming.h"
}



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

   void set_dof_id(DOF_Id id);
   void set_dof_type(DOF_Type type);

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
   DOF_Id dof_id = DOF_Id::INVALID;
   DOF_Type dof_type = DOF_Type::INVALID;

   const float SCALING_FACTOR_ANGULAR_POS = 100.0f;
   const float SCALING_FACTOR_ANGULAR_VEL = 100.0f;
   const float SCALING_FACTOR_FORCE = 100.0f;
   const float SCALING_FACTOR_LINEAR_POS = 50.0f;
   const float SCALING_FACTOR_LINEAR_VEL = 50.0f;
};

