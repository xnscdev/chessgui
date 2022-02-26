#include "boardwidget.h"
#include "promotiondialog.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>

BoardWidgetBackend::BoardWidgetBackend(GameVariant &game, QWidget *parent) : game(game), QWidget(parent) {
  reset();
}

void BoardWidgetBackend::reset() {
  canMove = true;
  turn = true;
  historyMove = -1;
  history.clear();
  highlightedTile.setX(-1);
  moveFromTile.setX(-1);
  prevSelectedPiece.setX(-1);
  selectedPiece.setX(-1);
  ep.setX(-1);
  availableTiles.clear();
  availableMovesMap.clear();

  position.clear();
  position.resize(game.size.height());
  for (auto &row : position) {
    row.resize(game.size.width());
  }
  game.setup(position);
  history.append({position, {{-1, 0}, {-1, 0}}});
  update();
}

void BoardWidgetBackend::toFirstMove() {
  if (history.size() > 1)
    historyMove = 0;
  update();
}

void BoardWidgetBackend::toPrevMove() {
  if (historyMove > 0)
    historyMove--;
  else if (historyMove == -1)
    historyMove = static_cast<int>(history.size()) - 2;
  update();
}

void BoardWidgetBackend::toNextMove() {
  if (historyMove != -1 && ++historyMove == history.size() - 1)
    historyMove = -1;
  update();
}

void BoardWidgetBackend::toLastMove() {
  historyMove = -1;
  update();
}

void BoardWidgetBackend::toMove(int move) {
  if (move >= history.size() - 1)
    historyMove = -1;
  else
    historyMove = move;
  update();
}

void BoardWidgetBackend::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  int xStep = width() / game.size.width();
  int yStep = height() / game.size.height();
  painter.setPen(Qt::lightGray);

  QPoint toHighlight;
  QPoint fromHighlight;
  if (historyMove == -1) {
    toHighlight = highlightedTile;
    fromHighlight = moveFromTile;
  }
  else {
    toHighlight = history[historyMove].prevMove.to;
    fromHighlight = history[historyMove].prevMove.from;
  }

  for (int x = 0; x < game.size.width(); x++) {
    for (int y = 0; y < game.size.height(); y++) {
      QColor color;
      QPoint point(x, game.size.height() - y - 1);
      if (point == toHighlight || point == fromHighlight)
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

  if (historyMove == -1)
    drawPosition(painter, position);
  else
    drawPosition(painter, history[historyMove].pos);
}

void BoardWidgetBackend::mousePressEvent(QMouseEvent *event) {
  if (!canMove || historyMove != -1)
    return;
  if (event->button() == Qt::LeftButton) {
    selectedPiece = selectedTile(event->pos());
    if (movablePieceAt(selectedPiece) && prevSelectedPiece.x() == -1) {
      highlightedTile = selectedPiece;
      showAvailableMoves();
    }
  }
}

void BoardWidgetBackend::mouseReleaseEvent(QMouseEvent *event) {
  if (!canMove || historyMove != -1)
    return;
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
      if (movablePieceAt(selectedPiece) && doMove(tile))
        highlightedTile = tile;
      else {
        availableTiles.clear();
        availableMovesMap.clear();
      }
      update();
    }
  }
}

QPoint BoardWidgetBackend::selectedTile(QPoint pos) {
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

void BoardWidgetBackend::drawPosition(QPainter &painter, GamePosition &pos) {
  int xStep = width() / game.size.width();
  int yStep = height() / game.size.height();
  for (int x = 0; x < game.size.width(); x++) {
    for (int y = 0; y < game.size.height(); y++) {
      Piece *piece = pos[y][x].piece;
      qreal tx = xStep * (orientation ? game.size.width() - x - 1 : x);
      qreal ty = yStep * (orientation ? y : game.size.height() - y - 1);
      if (piece) {
        QRectF target(tx, ty, xStep, yStep);
        painter.drawImage(target, piece->icon(pos[y][x].white), Piece::iconBox);
      }
    }
  }
}

void BoardWidgetBackend::showAvailableMoves() {
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

bool BoardWidgetBackend::doMove(QPoint to) {
  if (availableMovesMap.contains(to)) {
    Move move = availableMovesMap[to];
    GamePiece &fromPiece = position[move.from.y()][move.from.x()];
    GamePiece &toPiece = position[move.to.y()][move.to.x()];
    Piece *promotionPiece = nullptr;
    if ((turn ? game.size.height() - move.to.y() : move.to.y() + 1) <= fromPiece.piece->promotes)
      promotionPiece = promotePiece(fromPiece.piece);
    emit moveMade(game.moveName(position, move, ep, promotionPiece, turn));

    toPiece.piece = fromPiece.piece;
    toPiece.white = fromPiece.white;
    toPiece.moved = true;
    fromPiece.piece = nullptr;
    if (promotionPiece)
      toPiece.piece = promotionPiece;
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
    history.append({position, move});
    moveFromTile = move.from;
    update();
    findCheckmate();
    return true;
  }
  return false;
}

bool BoardWidgetBackend::movablePieceAt(QPoint tile) {
  return position[tile.y()][tile.x()].piece && position[tile.y()][tile.x()].white == turn;
}

Piece *BoardWidgetBackend::promotePiece(Piece *oldPiece) {
  PromotionDialog dialog(oldPiece, game, turn);
  dialog.exec();
  return dialog.selectedPiece;
}

void BoardWidgetBackend::findCheckmate() {
  for (int y = 0; y < game.size.height(); y++) {
    for (int x = 0; x < game.size.width(); x++) {
      GamePiece &piece = position[y][x];
      if (piece.piece && piece.white == turn) {
        QHash<QPoint, Move> moves = availableMoves(position, ep, {x, y});
        QHashIterator<QPoint, Move> it(moves);
        while (it.hasNext()) {
          it.next();
          if (legalPosition(positionAfterMove(position, it.value()), turn))
            return;
        }
      }
    }
  }
  canMove = false;
  metadata.result = turn ? "0-1" : "1-0";
  QMessageBox box;
  box.setText("Game Over");
  box.setInformativeText(QString(turn ? "Black" : "White") + " wins");
  box.exec();
}

BoardWidget::BoardWidget(QWidget *parent)
    : AspectRatioWidget(new BoardWidgetBackend(*loadedVariant), static_cast<float>(loadedVariant->size.width()),
                        static_cast<float>(loadedVariant->size.height()), parent) {
  backend = dynamic_cast<BoardWidgetBackend *>(widget());
  connect(backend, &BoardWidgetBackend::moveMade, this, &BoardWidget::receiveMoveMade);
}

QString BoardWidget::metadataPGN() const {
  QString str;
  str += "[Event \"" + backend->metadata.event + "\"]\n";
  str += "[Site \"" + backend->metadata.site + "\"]\n";

  str += "[Date \"";
  str += QString::number(backend->metadata.date.year()).rightJustified(4, '0');
  str += '.';
  str += QString::number(backend->metadata.date.month()).rightJustified(2, '0');
  str += '.';
  str += QString::number(backend->metadata.date.day()).rightJustified(2, '0');
  str += "\"]\n";

  str += "[Round \"" + backend->metadata.round + "\"]\n";
  str += "[White \"" + backend->metadata.whitePlayer + "\"]\n";
  str += "[Black \"" + backend->metadata.blackPlayer + "\"]\n";
  str += "[Result \"" + backend->metadata.result + "\"]\n\n";
  return str;
}

int BoardWidget::historyMove() {
  int move = backend->historyMove;
  if (move == -1)
    move = static_cast<int>(backend->history.size()) - 1;
  return move;
}

void BoardWidget::receiveMoveMade(const QString &move) {
  emit moveMade(move);
}
