#include "aspectratiowidget.h"
#include <QResizeEvent>
#include <QSpacerItem>

AspectRatioWidget::AspectRatioWidget(QWidget *widget, float aspectWidth, float aspectHeight, QWidget *parent)
    : mWidget(widget), aspectWidth(aspectWidth), aspectHeight(aspectHeight), QWidget(parent) {
  layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
  layout->addItem(new QSpacerItem(0, 0));
  layout->addWidget(widget);
  layout->addItem(new QSpacerItem(0, 0));
}

void AspectRatioWidget::resizeEvent(QResizeEvent *event) {
  float aspectRatio = static_cast<float>(event->size().width()) / static_cast<float>(event->size().height());
  int widgetStretch;
  int outerStretch;
  if (aspectRatio > aspectWidth / aspectHeight) {
    layout->setDirection(QBoxLayout::LeftToRight);
    widgetStretch = static_cast<int>(static_cast<float>(height()) * (aspectWidth / aspectHeight));
    outerStretch = static_cast<int>(lround(static_cast<float>(event->size().width() - widgetStretch) / 2.0f + 0.5f));
  }
  else {
    layout->setDirection(QBoxLayout::TopToBottom);
    widgetStretch = static_cast<int>(static_cast<float>(width()) * (aspectHeight / aspectWidth));
    outerStretch = static_cast<int>(lround(static_cast<float>(event->size().height() - widgetStretch) / 2.0f + 0.5f));
  }
  layout->setStretch(0, outerStretch);
  layout->setStretch(1, widgetStretch);
  layout->setStretch(2, outerStretch);
}
