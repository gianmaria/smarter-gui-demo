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

void Axis_Widget::set_axis_name(const QString& new_name)
{
   ui->axis_name->setText(new_name);
}

