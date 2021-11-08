#include "axis_widget.h"
#include "ui_axis_widget.h"

QString Axis_Widget::DOF_Type_to_str(DOF_Type type)
{
   switch (type)
   {
      case Axis_Widget::DOF_Type::LINEAR:
         return "LINEAR";
      case Axis_Widget::DOF_Type::ROTATIONAL:
         return "ROTATIONAL";
      default:
         return "UNKNOWN DOF_Type";
   }
}

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
   if (dof_type == DOF_Type::LINEAR)
      ui->axis_pos->setMinimum(value * SCALING_FACTOR_LINEAR_POS);
   else if(dof_type == DOF_Type::ROTATIONAL)
      ui->axis_pos->setMinimum(value * SCALING_FACTOR_ANGULAR_POS);
   else
      ui->axis_pos->setMinimum(69420);
}

void Axis_Widget::set_axis_pos_max(int value)
{
   if (dof_type == DOF_Type::LINEAR)
      ui->axis_pos->setMaximum(value * SCALING_FACTOR_LINEAR_POS);
   else if(dof_type == DOF_Type::ROTATIONAL)
      ui->axis_pos->setMaximum(value * SCALING_FACTOR_ANGULAR_POS);
   else
      ui->axis_pos->setMaximum(69420);
}

void Axis_Widget::set_axis_force_min(int value)
{
   ui->axis_force->setMinimum(value * SCALING_FACTOR_FORCE);
}

void Axis_Widget::set_axis_force_max(int value)
{
   ui->axis_force->setMaximum(value * SCALING_FACTOR_FORCE);
}

void Axis_Widget::set_axis_vel_min(int value)
{
   if (dof_type == DOF_Type::LINEAR)
      ui->axis_velocity->setMinimum(value * SCALING_FACTOR_LINEAR_VEL);
   else if(dof_type == DOF_Type::ROTATIONAL)
      ui->axis_velocity->setMinimum(value * SCALING_FACTOR_ANGULAR_VEL);
   else
      ui->axis_velocity->setMinimum(69420);
}

void Axis_Widget::set_axis_vel_max(int value)
{
   if (dof_type == DOF_Type::LINEAR)
      ui->axis_velocity->setMaximum(value * SCALING_FACTOR_LINEAR_VEL);
   else if(dof_type == DOF_Type::ROTATIONAL)
      ui->axis_velocity->setMaximum(value * SCALING_FACTOR_ANGULAR_VEL);
   else
      ui->axis_velocity->setMaximum(69420);

//   if (dof_id==0) qInfo() << "set_axis_vel_max" << ui->axis_velocity->maximum();
}

void Axis_Widget::set_axis_name(const QString& new_name)
{
   ui->axis_name->setText(new_name);
}

void Axis_Widget::set_dof_id(int id)
{
   dof_id = id;
}

void Axis_Widget::set_dof_type(DOF_Type type)
{
   dof_type = type;
   ui->axis_type->setText(QString("Type: %1").arg(DOF_Type_to_str(dof_type)));
}

void Axis_Widget::set_axis_pos(int pos)
{
   ui->axis_pos->setValue(pos);
}

void Axis_Widget::set_axis_vel(int vel)
{
//   if (dof_id == 0) qInfo() << "set_axis_vel:" << vel;
   ui->axis_velocity->setValue(vel);
}

void Axis_Widget::set_axis_force(int force)
{
   ui->axis_force->setValue(force);
}

void Axis_Widget::update_4dof(smarter_msg_4dof msg)
{
   pos_vel_force& pvf = msg.pvf[dof_id];

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

   ui->axis_pos_lbl->setText(QString::asprintf("%+06.2f", f_val));
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

   ui->label_velocity->setText(QString::asprintf("Velocity: %.2fmm/s", f_val));
}


void Axis_Widget::on_axis_force_valueChanged(int value)
{
   float f_val = static_cast<float>(value);

   f_val /= SCALING_FACTOR_FORCE;

   ui->label_force->setText(QString::asprintf("Force: %.2fN", f_val));
}

