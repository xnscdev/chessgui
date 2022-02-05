#include "boardwidget.h"

#include <QResizeEvent>
#include <QSpacerItem>

BoardWidget::BoardWidget(QWidget *parent) : QWidget(parent) {
  aspectWidth = 1;
  aspectHeight = 1;
  view = new QGraphicsView(this);

  layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
  layout->addItem(new QSpacerItem(0, 0));
  layout->addWidget(view);
  layout->addItem(new QSpacerItem(0, 0));
}

void BoardWidget::resizeEvent(QResizeEvent *event) {
  float aspectRatio = static_cast<float>(event->size().width()) / static_cast<float>(event->size().height());
  int widgetStretch;
  int outerStretch;
  if (aspectRatio > aspectWidth / aspectHeight) {
    layout->setDirection(QBoxLayout::LeftToRight);
    widgetStretch = static_cast<int>(static_cast<float>(height()) * (aspectWidth / aspectHeight));
    outerStretch = static_cast<int>(lround(static_cast<float>(width() - widgetStretch) / 2.0f + 0.5f));
  }
  else {
    layout->setDirection(QBoxLayout::TopToBottom);
    widgetStretch = static_cast<int>(static_cast<float>(width()) * (aspectHeight / aspectWidth));
    outerStretch = static_cast<int>(lround(static_cast<float>(height() - widgetStretch) / 2.0f + 0.5f));
  }
  layout->setStretch(0, outerStretch);
  layout->setStretch(1, widgetStretch);
  layout->setStretch(2, outerStretch);
}

