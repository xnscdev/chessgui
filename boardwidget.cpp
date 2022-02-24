#include "boardwidget.h"
#include "promotiondialog.h"
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
          if (doMove(tile)) {
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
      if (movablePieceAt(selectedPiece) && doMove(tile)) {
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
  availableMovesMap = availableMoves(position, ep, selectedPiece);
  QMutableHashIterator<QPoint, Move> it(availableMovesMap);
  while (it.hasNext()) {
    it.next();
    if (!legalPosition(positionAfterMove(position, it.value()), turn)) {
      it.remove();
      continue;
    }
    availableTiles.append(it.value().to);
  }
  update();
}

bool BoardWidget::BoardWidgetBackend::doMove(QPoint to) {
  if (availableMovesMap.contains(to)) {
    Move move = availableMovesMap[to];
    GamePiece &fromPiece = position[move.from.y()][move.from.x()];
    GamePiece &toPiece = position[move.to.y()][move.to.x()];
    toPiece.piece = fromPiece.piece;
    toPiece.white = fromPiece.white;
    toPiece.moved = true;
    fromPiece.piece = nullptr;
    if (game.size.height() - move.to.y() <= toPiece.piece->promotes)
      promotePiece(toPiece);
    if (move.capture.x() != -1)
      position[move.capture.y()][move.capture.x()].piece = nullptr;
    if (move.castle.x() != -1) {
      GamePiece &newPos = position[move.ep.y()][move.ep.x()];
      GamePiece &oldPos = position[move.castle.y()][move.castle.x()];
      newPos.piece = oldPos.piece;
      newPos.white = oldPos.white;
      newPos.moved = true;
      oldPos.piece = nullptr;
    }
    else {
      if (move.ep.x() != -1)
        ep = move.ep;
      else
        ep.setX(-1);
    }
    turn = !turn;
    availableTiles.clear();
    availableMovesMap.clear();
    update();
    return true;
  }
  return false;
}

bool BoardWidget::BoardWidgetBackend::movablePieceAt(QPoint tile) {
  return position[tile.y()][tile.x()].piece && position[tile.y()][tile.x()].white == turn;
}

void BoardWidget::BoardWidgetBackend::promotePiece(GamePiece &piece) {
  PromotionDialog dialog(piece.piece, game, turn);
  dialog.exec();
  piece.piece = dialog.selectedPiece;
}
