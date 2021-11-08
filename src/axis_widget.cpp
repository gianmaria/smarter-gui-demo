#include "axis_widget.h"
#include "ui_axis_widget.h"

Axis_Widget::Axis_Widget(QWidget *parent) :
   QWidget(parent),
   ui(new Ui::Axis_Widget)
{
   ui->setupUi(this);
}

Axis_Widget::~Axis_Widget()
{
   delete ui;
}

void Axis_Widget::set_axis_pos_min(int value)
{
   ui->axis_pos->setMinimum(value);
}

void Axis_Widget::set_axis_pos_max(int value)
{
   ui->axis_pos->setMaximum(value);
}

void Axis_Widget::set_axis_force_min(int value)
{
   ui->axis_force->setMinimum(value);
}

void Axis_Widget::set_axis_force_max(int value)
{
   ui->axis_force->setMaximum(value);
}

void Axis_Widget::set_axis_vel_min(int value)
{
   ui->axis_velocity->setMinimum(value * SCALING_FACTOR_ANGULAR_VEL);
}

void Axis_Widget::set_axis_vel_max(int value)
{
   ui->axis_velocity->setMaximum(value);
}

void Axis_Widget::set_axis_name(const QString& new_name)
{
   ui->axis_name->setText(new_name);
}

void Axis_Widget::set_axis_type(const QString& type)
{
   ui->axis_type->setText(QString("Type: %1").arg(type));
}

void Axis_Widget::set_dof_id(int id)
{
   dof_id = id;
}

int Axis_Widget::get_dof_id()
{
   return dof_id;
}

void Axis_Widget::set_dof_type(DOF_Type type)
{
   dof_type = type;
}

void Axis_Widget::set_axis_pos(int pos)
{
   ui->axis_pos->setValue(pos);
}

void Axis_Widget::set_axis_vel(int vel)
{
   ui->axis_velocity->setValue(vel / SCALING_FACTOR_ANGULAR_VEL);
}

void Axis_Widget::set_axis_force(int force)
{
   ui->axis_force->setValue(force / SCALING_FACTOR_FORCE);
}

void Axis_Widget::update_4dof(smarter_msg_4dof msg)
{
   pos_vel_force& pvf = msg.pvf[dof_id];

   if (pvf.dof_type == DOF_Type::LINEAR)
   {
      dof_type = DOF_Type::LINEAR;
      set_axis_type("Linear");
   }
   else if (pvf.dof_type == DOF_Type::ROTATIONAL)
   {
      dof_type = DOF_Type::ROTATIONAL;
      set_axis_type("Rotational");
   }
   else
   {
      dof_type = DOF_Type::UNKNOWN;
      set_axis_type("Unknown");
   }

   set_axis_pos(pvf.position);
   set_axis_vel(pvf.velocity);
   set_axis_force(pvf.force);
}


void Axis_Widget::on_axis_pos_valueChanged(int value)
{
   float f_val = static_cast<float>(value);

   if (dof_type == DOF_Type::LINEAR)
      f_val /= SCALING_FACTOR_LINEAR_POS;
   else if (dof_type == DOF_Type::ROTATIONAL)
      f_val /= SCALING_FACTOR_ANGULAR_POS;
   else
      f_val = 99.99f;

   ui->axis_pos_lbl->setText(QString::asprintf("%+06.2f", f_val));
}

