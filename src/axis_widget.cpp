#include "axis_widget.h"
#include "ui_axis_widget.h"

#include <QtMath>

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


void Axis_Widget::set_axis_name(const QString& new_name)
{
   ui->axis_name->setText(new_name);
}

void Axis_Widget::set_dof_id(DOF_Id id)
{
   dof_id = id;
}

void Axis_Widget::set_dof_type(DOF_Type type)
{
   dof_type = type;
   ui->axis_type->setText(QString("Type: %1").arg(to_str(dof_type)));
}

void Axis_Widget::set_axis_pos(int pos)
{
   if (pos > ui->axis_pos->maximum())
      ui->axis_pos->setMaximum(pos + ui->axis_pos->tickInterval());
   else if (pos < ui->axis_pos->minimum())
      ui->axis_pos->setMinimum(pos - ui->axis_pos->tickInterval());

   ui->axis_pos->setValue(pos);
}



void Axis_Widget::set_axis_force(int force)
{
   int force_abs = qFabs<int>(force);

   if (force_abs > ui->axis_force->maximum())
      ui->axis_force->setMaximum(force_abs);

   ui->axis_force->setValue(force_abs);
}

void Axis_Widget::set_axis_vel(int vel)
{
   int vel_abs = qFabs<int>(vel);

   if (vel_abs > ui->axis_velocity->maximum())
      ui->axis_velocity->setMaximum(vel_abs);

   ui->axis_velocity->setValue(vel_abs);
}

void Axis_Widget::refresh_4dof(smarter_msg_4dof msg)
{
   pos_vel_force& pvf = msg.pvf[static_cast<SM_uchar>(dof_id)];

   set_dof_type(static_cast<DOF_Type>(pvf.dof_type));

   set_axis_pos(pvf.position);
   set_axis_vel(pvf.velocity);
   set_axis_force(pvf.force);
}

void Axis_Widget::refresh_dof(pos_vel_force pvf)
{
   set_dof_type(static_cast<DOF_Type>(pvf.dof_type));

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

   if (dof_type == DOF_Type::ROTATIONAL)
      ui->axis_pos_lbl->setText(QString::asprintf("%+5.2f °", f_val));
   else if (dof_type == DOF_Type::LINEAR)
      ui->axis_pos_lbl->setText(QString::asprintf("%+5.2f mm", f_val));
   else
      ui->axis_pos_lbl->setText("???");
}


void Axis_Widget::on_axis_velocity_valueChanged(int value)
{
   float f_val = static_cast<float>(value);

   if (dof_type == DOF_Type::LINEAR)
      f_val /= SCALING_FACTOR_LINEAR_VEL;
   else if (dof_type == DOF_Type::ROTATIONAL)
      f_val /= SCALING_FACTOR_ANGULAR_VEL;
   else
      f_val = 99.99f;

   if (dof_type == DOF_Type::ROTATIONAL)
      ui->label_velocity->setText(QString::asprintf("Velocity: %5.2f °/s", f_val));
   else if (dof_type == DOF_Type::LINEAR)
      ui->label_velocity->setText(QString::asprintf("Velocity: %5.2f mm/s", f_val));
   else
      ui->label_velocity->setText("???");

}


void Axis_Widget::on_axis_force_valueChanged(int value)
{
   float f_val = static_cast<float>(value);

   f_val /= SCALING_FACTOR_FORCE;

   ui->label_force->setText(QString::asprintf("Force: %5.2f N", f_val));
}

