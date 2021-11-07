#include "custom_slider.h"

#include <QFontMetrics>
#include <QPainter>

Custom_Slider::Custom_Slider(QWidget* parent) :
   QSlider(parent)
{
   setOrientation(Qt::Horizontal);
   setMinimum(0);
   setMaximum(100);
   setValue(34);

   setSingleStep(1);
   setTickPosition(QSlider::TicksAbove);
   setTickInterval(10);
}


void Custom_Slider::paintEvent(QPaintEvent* event)
{
   QSlider::paintEvent(event);
#if 1
   QPainter painter(this);
   painter.setPen(QPen(Qt::black));

   auto rect = this->geometry();
   rect.setWidth(rect.width() - 10);


   if (this->orientation() == Qt::Horizontal) {

      QFontMetrics fontMetrics = QFontMetrics(this->font());
      int fontHeight = fontMetrics.height();

      int numTicks = (maximum() - minimum())/tickInterval();

      for (int i=0; i<=numTicks; i++){

         int tickNum = minimum() + (tickInterval() * i);

         auto offset = (fontMetrics.horizontalAdvance(QString::number(tickNum)) / 2);
         auto tickX = ((rect.width()/numTicks) * i) - offset;
         auto tickY = rect.height() - fontHeight + 10;

         painter.drawText(QPoint(tickX, tickY),
                          QString::number(tickNum));
      }

   } else if (this->orientation() == Qt::Vertical) {

      //do something else for vertical here, I haven't implemented it because I don't need it
   }
   else
   {
      return;
   }

   painter.drawRect(rect);
#endif

}
