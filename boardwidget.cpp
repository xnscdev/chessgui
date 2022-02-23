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
  painter.setPen(Qt::lightGray);
  for (int x = 0; x < game.size.width(); x++) {
    for (int y = 0; y < game.size.height(); y++) {
      QColor color;
      QPoint point(x, game.size.height() - y - 1);
      if (point == highlightedTile)
        color = highlightColor;
      else if ((x + y) & 1)
        color = darkColor;
      else
        color = lightColor;
      painter.fillRect(x * xStep, y * xStep, xStep, yStep, color);
      if (availableTiles.contains(point))
        painter.drawEllipse(QPoint(x * xStep + xStep / 2, y * yStep + yStep / 2), xStep * 7 / 16, yStep * 7 / 16);
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
    if (movablePieceAt(selectedPiece) && prevSelectedPiece.x() == -1) {
      highlightedTile = selectedPiece;
      showAvailableMoves();
    }
  }
}

void BoardWidget::BoardWidgetBackend::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    QPoint tile = selectedTile(event->pos());
    if (tile == selectedPiece) {
      if (prevSelectedPiece.x() == -1) {
        if (movablePieceAt(selectedPiece))
          prevSelectedPiece = tile;
      }
      else if (tile != prevSelectedPiece) {
        if (movablePieceAt(prevSelectedPiece)) {
          if (attemptMove(prevSelectedPiece, tile)) {
            highlightedTile = selectedPiece;
            prevSelectedPiece.setX(-1);
          }
          else if (movablePieceAt(selectedPiece)) {
            highlightedTile = selectedPiece;
            prevSelectedPiece = selectedPiece;
            showAvailableMoves();
          }
        }
      }
    }
    else {
      if (movablePieceAt(selectedPiece) && attemptMove(selectedPiece, tile)) {
        highlightedTile = tile;
        update();
      }
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
  tileY = game.size.height() - tileY - 1;
  if (orientation) {
    tileX = game.size.width() - tileX - 1;
    tileY = game.size.height() - tileY - 1;
  }
  return {tileX, tileY};
}

void BoardWidget::BoardWidgetBackend::showAvailableMoves() {
  availableTiles.clear();
  for (auto &move : availableMoves(position, selectedPiece))
    availableTiles.append(move.to);
  update();
}

bool BoardWidget::BoardWidgetBackend::attemptMove(QPoint from, QPoint to) {
  bool moved = false;
  if (availableTiles.contains(to)) {
    GamePiece &fromPiece = position[from.y()][from.x()];
    GamePiece &toPiece = position[to.y()][to.x()];
    toPiece.piece = fromPiece.piece;
    toPiece.white = fromPiece.white;
    fromPiece.piece = nullptr;
    toPiece.moved = true;
    turn = !turn;
    moved = true;
  }
  availableTiles.clear();
  update ();
  return moved;
}

bool BoardWidget::BoardWidgetBackend::movablePieceAt(QPoint tile) {
  return position[tile.y()][tile.x()].piece && position[tile.y()][tile.x()].white == turn;
}
