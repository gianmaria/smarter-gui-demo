#include "joystick_widget.h"
#include "ui_joystick_widget.h"

Joystick_Widget::Joystick_Widget(QWidget *parent) :
   QWidget(parent),
   ui(new Ui::Joystick_Widget)
{
   ui->setupUi(this);
}

Joystick_Widget::~Joystick_Widget()
{
   delete ui;
}
