#include "boardwidget.h"
#include <QMouseEvent>
#include <QPainter>

BoardWidget::BoardWidgetBackend::BoardWidgetBackend(GameVariant &game, QWidget *parent) : game(game), QWidget(parent) {
  position.resize(game.size.height());
  for (auto &row : position) {
    row.resize(game.size.width());
  }
  game.setup(position);
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

  for (int x = 0; x < game.size.width(); x++) {
    for (int y = 0; y < game.size.height(); y++) {
      Piece *piece = position[y][x].piece;
      qreal tx = xStep * (orientation ? game.size.width() - x - 1 : x);
      qreal ty = yStep * (orientation ? y : game.size.height() - y - 1);
      if (piece) {
        QRectF target(tx, ty, xStep, yStep);
        painter.drawImage(target, piece->icon(position[y][x].white), Piece::iconBox);
      }
    }
  }
}

void BoardWidget::BoardWidgetBackend::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton)
    qDebug() << event->pos();
}

void BoardWidget::BoardWidgetBackend::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton)
    qDebug() << event->pos();
}
