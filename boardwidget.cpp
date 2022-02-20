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
      QColor color;
      if (QPoint(x, y) == highlightedTile)
        color = highlightColor;
      else if ((x + y) & 1)
        color = darkColor;
      else
        color = lightColor;
      painter.fillRect(x * xStep, y * xStep, xStep, yStep, color);
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
  if (event->button() == Qt::LeftButton) {
    selectedPiece = selectedTile(event->pos());
  }
}

void BoardWidget::BoardWidgetBackend::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    QPoint tile = selectedTile(event->pos());
    if (tile == selectedPiece) {
      if (prevSelectedPiece.x() == -1) {
        if (pieceAt(selectedPiece).piece) {
          highlightedTile = selectedPiece;
          prevSelectedPiece = tile;
          update();
        }
      }
      else if (tile != prevSelectedPiece) {
        if (pieceAt(prevSelectedPiece).piece) {
          highlightedTile = selectedPiece;
          attemptMove(prevSelectedPiece, tile);
          prevSelectedPiece.setX(-1);
        }
      }
    }
    else {
      highlightedTile = tile;
      attemptMove(selectedPiece, tile);
    }
  }
}

QPoint BoardWidget::BoardWidgetBackend::selectedTile(QPoint pos) {
  int xStep = width() / game.size.width();
  int yStep = height() / game.size.height();
  int tileX = pos.x() / xStep;
  int tileY = pos.y() / yStep;
  if (tileX >= game.size.width())
    tileX = game.size.width() - 1;
  if (tileY >= game.size.height())
    tileY = game.size.height() - 1;
  if (orientation) {
    tileX = game.size.width() - tileX - 1;
    tileY = game.size.height() - tileY - 1;
  }
  return {tileX, tileY};
}

void BoardWidget::BoardWidgetBackend::attemptMove(QPoint from, QPoint to) {
  GamePiece &fromPiece = pieceAt(from);
  GamePiece &toPiece = pieceAt(to);
  toPiece.piece = fromPiece.piece;
  toPiece.white = fromPiece.white;
  fromPiece.piece = nullptr;
  update();
}

GamePiece &BoardWidget::BoardWidgetBackend::pieceAt(QPoint tile) {
  return position[game.size.height() - tile.y() - 1][tile.x()];
}
