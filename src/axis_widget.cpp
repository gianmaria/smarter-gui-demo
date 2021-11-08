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
   ui->axis_velocity->setMinimum(value);
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

void Axis_Widget::set_axis_pos(int pos)
{
   ui->axis_pos->setValue(pos);
}

void Axis_Widget::set_axis_vel(int vel)
{
   ui->axis_velocity->setValue(vel);
}

void Axis_Widget::set_axis_force(int force)
{
   ui->axis_force->setValue(force);
}


void Axis_Widget::on_axis_pos_valueChanged(int value)
{
   float f_val = static_cast<float>(value) / 100.0f;
   ui->axis_pos_lbl->setText(QString::asprintf("%+06.2f", f_val));
}

