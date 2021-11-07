#pragma once

#include <QObject>
#include <QSlider>

class Custom_Slider : public QSlider
{
   Q_OBJECT

public:
   explicit Custom_Slider(QWidget* parent = nullptr);


   // QWidget interface
protected:
   virtual void paintEvent(QPaintEvent* event) override;
};

