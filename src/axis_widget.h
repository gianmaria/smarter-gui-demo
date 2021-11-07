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

   void set_axis_name(const QString& new_name);

private:
   Ui::Axis_Widget *ui;
};

