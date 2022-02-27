#include "boardwidget.h"
#include "promotiondialog.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QtConcurrent>

BoardWidgetBackend::BoardWidgetBackend(GameVariant &game, QWidget *parent) : game(game), QWidget(parent) {
  reset();
  gameRunning = false;
  if (settings.value("timeControl").toBool()) {
    whiteTimer = new QTimer(this);
    blackTimer = new QTimer(this);
    connect(whiteTimer, &QTimer::timeout, this, &BoardWidgetBackend::whiteTimerTick);
    connect(blackTimer, &QTimer::timeout, this, &BoardWidgetBackend::blackTimerTick);
    whiteTime = settings.value("baseTime").toInt() * 60000;
    blackTime = whiteTime;
    moveBonus = settings.value("moveBonus").toInt() * 1000;
  }
}

void BoardWidgetBackend::reset() {
  gameRunning = true;
  turn = true;
  pgnResult = "*";
  historyMove = -1;
  history.clear();
  highlightedTile.setX(-1);
  moveFromTile.setX(-1);
  moveToTile.setX(-1);
  prevSelectedPiece.setX(-1);
  selectedPiece.setX(-1);
  ep.setX(-1);
  availableTiles.clear();
  availableMovesMap.clear();
  uciMoveString.clear();

  position.clear();
  position.resize(game.size.height());
  for (auto &row : position) {
    row.resize(game.size.width());
  }
  game.setup(position);
  history.append({position, {{-1, 0}, {-1, 0}}});
  update();
}

void BoardWidgetBackend::newGame() {
  delete whiteInputMethod;
  delete blackInputMethod;
  whiteInputMethod = createMoveInputMethod("whitePlayer", true);
  blackInputMethod = createMoveInputMethod("blackPlayer", false);
  reset();
  whiteInputMethod->reset(settings.value("whiteELOEnabled").toBool(), settings.value("whiteELO", 1800).toInt());
  blackInputMethod->reset(settings.value("blackELOEnabled").toBool(), settings.value("blackELO", 1800).toInt());
  whiteInputMethod->start(uciMoveString);
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
  QPoint altHighlight;
  if (historyMove == -1) {
    toHighlight = moveToTile;
    fromHighlight = moveFromTile;
    altHighlight = highlightedTile;
  }
  else {
    toHighlight = history[historyMove].prevMove.to;
    fromHighlight = history[historyMove].prevMove.from;
    altHighlight = {-1, 0};
  }

  for (int x = 0; x < game.size.width(); x++) {
    for (int y = 0; y < game.size.height(); y++) {
      QColor color;
      QPoint point(x, game.size.height() - y - 1);
      if (point == toHighlight || point == altHighlight)
        color = highlightColor;
      else if (point == fromHighlight)
        color = fromHighlightColor;
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
  if (!gameRunning || historyMove != -1)
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
  if (!gameRunning || historyMove != -1)
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
          if (tryMove(tile))
            highlightedTile = tile;
          else if (movablePieceAt(selectedPiece)) {
            highlightedTile = selectedPiece;
            prevSelectedPiece = selectedPiece;
            showAvailableMoves();
          }
        }
      }
    }
    else {
      if (movablePieceAt(selectedPiece) && tryMove(tile))
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

void BoardWidgetBackend::doMove(Move &move) {
  GamePiece &fromPiece = position[move.from.y()][move.from.x()];
  GamePiece &toPiece = position[move.to.y()][move.to.x()];
  Piece *promotionPiece = nullptr;
  if (!move.promote.isEmpty()) {
    QMapIterator<QString, QString> it(game.notation);
    while (it.hasNext()) {
      it.next();
      if (it.value() == move.promote) {
        promotionPiece = game.pieces[it.key()];
        break;
      }
    }
  }
  else if ((turn ? game.size.height() - move.to.y() : move.to.y() + 1) <= fromPiece.piece->promotes) {
    promotionPiece = promotePiece(fromPiece.piece);
    move.promote = game.notation[promotionPiece->name];
  }
  QString moveName = game.moveName(position, move, ep, promotionPiece, turn);
  uciMoveString += ' ' + move;
  emit moveMade(moveName);

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
  moveToTile = move.to;
  update();

  if (findGameEnd())
    return;
  if (turn)
    whiteInputMethod->start(uciMoveString);
  else
    blackInputMethod->start(uciMoveString);
}

bool BoardWidgetBackend::tryMove(QPoint to) {
  if (availableMovesMap.contains(to)) {
    highlightedTile = selectedPiece;
    prevSelectedPiece.setX(-1);
    Move move = availableMovesMap[to];
    doMove(move);
    return true;
  }
  return false;
}

bool BoardWidgetBackend::movablePieceAt(QPoint tile) {
  return position[tile.y()][tile.x()].piece && position[tile.y()][tile.x()].white == turn &&
         (turn ? whiteInputMethod : blackInputMethod)->manual();
}

Piece *BoardWidgetBackend::promotePiece(Piece *oldPiece) {
  PromotionDialog dialog(oldPiece, game, turn);
  dialog.exec();
  return dialog.selectedPiece;
}

bool BoardWidgetBackend::findGameEnd() {
  for (int y = 0; y < game.size.height(); y++) {
    for (int x = 0; x < game.size.width(); x++) {
      GamePiece &piece = position[y][x];
      if (piece.piece && piece.white == turn) {
        QHash<QPoint, Move> moves = availableMoves(position, ep, {x, y});
        QHashIterator<QPoint, Move> it(moves);
        while (it.hasNext()) {
          it.next();
          if (legalPosition(positionAfterMove(position, it.value()), turn))
            return false;
        }
      }
    }
  }
  QMessageBox box(this);
  box.setIcon(QMessageBox::Information);
  box.setText("Game Over");
  gameRunning = false;
  if (legalPosition(position, turn)) {
    pgnResult = "1/2-1/2";
    box.setInformativeText("Draw by stalemate");
  }
  else {
    pgnResult = turn ? "0-1" : "1-0";
    box.setInformativeText(turn ? "Black wins" : "White wins");
  }
  box.exec();
  return true;
}

MoveInputMethod *BoardWidgetBackend::createMoveInputMethod(const QString &key, bool white) {
  int index = settings.value(key).toInt();
  if (index < 2)
    return new MoveInputMethod;
  settings.beginReadArray("engines");
  settings.setArrayIndex(index - 2);
  QString cmd = settings.value("command").toString();
  settings.endArray();
  auto *method = new UCIMoveInputMethod(cmd, white);
  connect(method, &UCIMoveInputMethod::engineMoved, this, &BoardWidgetBackend::receiveEngineMove, Qt::QueuedConnection);
  return method;
}

void BoardWidgetBackend::receiveEngineMove(const QString &moveString) {
  QPoint from{moveString[0].toLatin1() - 'a', QString(moveString[1]).toInt() - 1};
  QPoint to{moveString[2].toLatin1() - 'a', QString(moveString[3]).toInt() - 1};
  QHash<QPoint, Move> moves = availableMoves(position, ep, from);
  QHashIterator<QPoint, Move> it(moves);
  while (it.hasNext()) {
    it.next();
    if (it.value().to == to) {
      Move move = it.value();
      if (moveString.length() == 5)
        move.promote = moveString[4];
      doMove(move);
      break;
    }
  }
}

void BoardWidgetBackend::whiteTimerTick() {
  whiteTime -= timerFreq;
}

void BoardWidgetBackend::blackTimerTick() {
  blackTime -= timerFreq;
}

void BoardWidgetBackend::closeEngines() {
  delete whiteInputMethod;
  delete blackInputMethod;
}

BoardWidget::BoardWidget(QWidget *parent)
    : AspectRatioWidget(new BoardWidgetBackend(*loadedVariant), static_cast<float>(loadedVariant->size.width()),
                        static_cast<float>(loadedVariant->size.height()), parent) {
  backend = dynamic_cast<BoardWidgetBackend *>(widget());
  connect(backend, &BoardWidgetBackend::moveMade, this, &BoardWidget::receiveMoveMade);
}

QString BoardWidget::playerName(QSettings &settings, const QString &key, int defaultValue) {
  int index = settings.value(key, defaultValue).toInt();
  switch (index) {
  case 0:
    return settings.value("humanPlayer1Name", "?").toString();
  case 1:
    return settings.value("humanPlayer2Name", "?").toString();
  default:
    settings.beginReadArray("engines");
    settings.setArrayIndex(index - 2);
    QString name = settings.value("name", "?").toString();
    settings.endArray();
    return name;
  }
}

QString BoardWidget::metadataPGN() const {
  QSettings settings;
  QString str;
  str += "[Event \"" + fillPGNTag(settings, "event") + "\"]\n";
  str += "[Site \"" + fillPGNTag(settings, "site") + "\"]\n";

  QDate date = settings.value("date", QDate::currentDate()).toDate();
  str += "[Date \"";
  str += QString::number(date.year()).rightJustified(4, '0');
  str += '.';
  str += QString::number(date.month()).rightJustified(2, '0');
  str += '.';
  str += QString::number(date.day()).rightJustified(2, '0');
  str += "\"]\n";

  str += "[Round \"" + fillPGNTag(settings, "round") + "\"]\n";
  str += "[White \"" + playerName(settings, "whitePlayer", 0) + "\"]\n";
  str += "[Black \"" + playerName(settings, "blackPlayer", 1) + "\"]\n";
  str += "[Result \"" + backend->pgnResult + "\"]\n\n";
  return str;
}

int BoardWidget::historyMove() const {
  int move = backend->historyMove;
  if (move == -1)
    move = static_cast<int>(backend->history.size()) - 1;
  return move;
}

QString BoardWidget::fillPGNTag(const QSettings &settings, const QString &key) const {
  QString value = settings.value(key).toString();
  return value.isEmpty() ? "?" : value;
}

QString BoardWidget::formattedTime(bool white) const {
  int ms = white ? backend->whiteTime : backend->blackTime;
  int sec = ms / 1000;
  ms %= 1000;
  int min = sec / 60;
  sec %= 60;
  int h = min / 60;
  min %= 60;
  return QString::number(h).rightJustified(2, '0') + ':' + QString::number(min).rightJustified(2, '0') + ':' +
         QString::number(sec).rightJustified(2, '0') + '.' + QString::number(ms).rightJustified(3, '0');
}

void BoardWidget::receiveMoveMade(const QString &move) {
  emit moveMade(move);
}
