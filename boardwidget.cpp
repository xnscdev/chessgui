#include "boardwidget.h"
#include <QMouseEvent>
#include <QPainter>

BoardWidget::BoardWidgetBackend::BoardWidgetBackend(GameVariant game, QWidget *parent) : game(game), QWidget(parent) {
}

void BoardWidget::BoardWidgetBackend::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  int xStep = width() / game.size.width();
  int yStep = height() / game.size.height();
  for (int x = 0; x < game.size.width(); x++) {
    for (int y = 0; y < game.size.height(); y++) {
      painter.fillRect(x * xStep, y * xStep, xStep, yStep, (x + y) & 1 ? darkColor : lightColor);
    }
  }

  QRectF target(0, 0, xStep, yStep);
  QRectF source(0, 0, 128, 128);
  QImage image(":/icons/white-king.png");
  painter.drawImage(target, image, source);
}

void BoardWidget::BoardWidgetBackend::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton)
    qDebug() << event->pos();
}

void BoardWidget::BoardWidgetBackend::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton)
    qDebug() << event->pos();
}
