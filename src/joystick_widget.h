#pragma once

#include <QWidget>

namespace Ui {
class Joystick_Widget;
}

class Joystick_Widget : public QWidget
{
   Q_OBJECT

public:
   explicit Joystick_Widget(QWidget *parent = nullptr);
   ~Joystick_Widget();

private:
   Ui::Joystick_Widget *ui;
};

