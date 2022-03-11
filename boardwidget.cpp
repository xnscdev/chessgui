#include "boardwidget.h"
#include "promotiondialog.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>

BoardWidgetBackend::BoardWidgetBackend(GameVariant &game, QWidget *parent) : game(game), QWidget(parent) {
  reset();
}

void BoardWidgetBackend::reset() {
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
  moveOccurrences.clear();
  lastIrreversibleMove = 0;
  whiteTime = 0;
  blackTime = 0;
  if (whiteTimer) {
    delete whiteTimer;
    whiteTimer = nullptr;
  }
  if (blackTimer) {
    delete blackTimer;
    blackTimer = nullptr;
  }

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
  delete evalEngine;
  whiteInputMethod = createMoveInputMethod("whitePlayer", true);
  blackInputMethod = createMoveInputMethod("blackPlayer", false);
  evalEngine = createEvalEngine();
  gameRunning = true;
  reset();

  emit evalBarUpdate(0, "0.00");
  if (settings.value("timeControl").toBool()) {
    whiteTimer = new QTimer(this);
    whiteTimer->setInterval(timerFreq);
    blackTimer = new QTimer(this);
    blackTimer->setInterval(timerFreq);
    connect(whiteTimer, &QTimer::timeout, this, &BoardWidgetBackend::whiteTimerTick);
    connect(blackTimer, &QTimer::timeout, this, &BoardWidgetBackend::blackTimerTick);
    whiteTime = settings.value("baseTime").toInt() * 60000;
    blackTime = whiteTime;
    moveBonus = settings.value("moveBonus").toInt() * 1000;
    emit whiteTimerTicked(whiteTime);
    emit blackTimerTicked(blackTime);

    whiteTimer->start();
    whiteInputMethod->reset(settings.value("whiteELOEnabled").toBool(), settings.value("whiteELO", 1800).toInt());
    blackInputMethod->reset(settings.value("blackELOEnabled").toBool(), settings.value("blackELO", 1800).toInt());
    whiteInputMethod->start(uciMoveString, whiteTime, blackTime, moveBonus);
  }
  else {
    emit whiteTimerTicked(-1);
    emit blackTimerTicked(-1);
  }
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

void BoardWidgetBackend::closeEngines() {
  delete whiteInputMethod;
  delete blackInputMethod;
  delete evalEngine;
}

void BoardWidgetBackend::whiteTimerTick() {
  whiteTime -= timerFreq;
  if (whiteTime <= 0) {
    whiteTime = 0;
    endGame("0-1", "Black wins by timeout");
  }
  emit whiteTimerTicked(whiteTime);
}

void BoardWidgetBackend::blackTimerTick() {
  blackTime -= timerFreq;
  if (blackTime <= 0) {
    blackTime = 0;
    endGame("1-0", "White wins by timeout");
  }
  emit blackTimerTicked(blackTime);
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
  if (position[move.to.y()][move.to.x()].piece || move.capture.x() != -1 || fromPiece.piece->name == "pawn")
    lastIrreversibleMove = 0;
  else
    lastIrreversibleMove++;
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
  if (whiteTimer || blackTimer) {
    if (turn) {
      blackTime += moveBonus - blackTimer->interval() + blackTimer->remainingTime();
      emit blackTimerTicked(blackTime);
      blackTimer->stop();
      whiteTimer->start();
    }
    else {
      whiteTime += moveBonus - whiteTimer->interval() + whiteTimer->remainingTime();
      emit whiteTimerTicked(whiteTime);
      whiteTimer->stop();
      blackTimer->start();
    }
  }
  if (evalEngine)
    evalEngine->startEval(uciMoveString, turn);
  if (turn)
    whiteInputMethod->start(uciMoveString, whiteTime, blackTime, moveBonus);
  else
    blackInputMethod->start(uciMoveString, whiteTime, blackTime, moveBonus);
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
  QString ima;
  QString imb;
  for (int y = 0; y < game.size.height(); y++) {
    for (int x = 0; x < game.size.width(); x++) {
      GamePiece &piece = position[y][x];
      if (piece.piece) {
        if (piece.white)
          ima += game.notation[piece.piece->name];
        else
          imb += game.notation[piece.piece->name];
      }
    }
  }
  if (findDrawIM(ima, imb)) {
    endGame("1/2-1/2", "Draw by insufficient material");
    return true;
  }

  for (int y = 0; y < game.size.height(); y++) {
    for (int x = 0; x < game.size.width(); x++) {
      GamePiece &piece = position[y][x];
      if (piece.piece && piece.white == turn) {
        QHash<QPoint, Move> moves = availableMoves(position, ep, {x, y});
        QHashIterator<QPoint, Move> it(moves);
        while (it.hasNext()) {
          it.next();
          if (legalPosition(positionAfterMove(position, it.value()), turn)) {
            if (++moveOccurrences[gameState()] >= 3)
              endGame("1/2-1/2", "Draw by repetition");
            else if (lastIrreversibleMove >= 100)
              endGame("1/2-1/2", "Draw by 50-move rule");
            else
              return false;
            return true;
          }
        }
      }
    }
  }
  if (legalPosition(position, turn))
    endGame("1/2-1/2", "Draw by stalemate");
  else
    endGame(turn ? "0-1" : "1-0", turn ? "Black wins" : "White wins");
  return true;
}

bool BoardWidgetBackend::findDrawIM(QString ima, QString imb) {
  std::sort(ima.begin(), ima.end());
  std::sort(imb.begin(), imb.end());
  if (imb.size() > ima.size()) {
    QString temp = imb;
    imb = ima;
    ima = temp;
  }
  return (imb == "k" && QList<QString>{"bk", "kn", "knn"}.contains(ima)) || (ima == "bk" && imb == "bk");
}

void BoardWidgetBackend::endGame(const QString &score, const QString &msg) {
  QMessageBox box(this);
  box.setIcon(QMessageBox::Information);
  box.setText("Game Over");
  box.setInformativeText(msg);
  gameRunning = false;
  pgnResult = score;
  if (whiteTimer) {
    delete whiteTimer;
    whiteTimer = nullptr;
  }
  if (blackTimer) {
    delete blackTimer;
    blackTimer = nullptr;
  }
  emit evalBarUpdate(gameEndScores[score], score);
  box.exec();
}

QString BoardWidgetBackend::gameState() const {
  QString str;
  for (int y = 0; y < game.size.height(); y++) {
    for (int x = 0; x < game.size.width(); x++) {
      Piece *piece = position[y][x].piece;
      if (piece)
        str += game.notation[piece->name];
      else
        str += '.';
    }
  }
  str += turn ? 'w' : 'b';
  return str;
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

UCIEngine *BoardWidgetBackend::createEvalEngine() {
  int index = settings.value("evalEngine").toInt();
  if (index < 1)
    return nullptr;
  settings.beginReadArray("engines");
  settings.setArrayIndex(index - 1);
  QString cmd = settings.value("command").toString();
  settings.endArray();
  auto *engine = new UCIEngine(cmd, true, true, settings.value("engineDepth", 25).toInt());
  connect(engine, &UCIEngine::evalBarUpdate, this, &BoardWidgetBackend::receiveEvalBarUpdate);
  return engine;
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

BoardWidget::BoardWidget(QWidget *parent)
    : AspectRatioWidget(new BoardWidgetBackend(*loadedVariant), static_cast<float>(loadedVariant->size.width()),
                        static_cast<float>(loadedVariant->size.height()), parent) {
  backend = dynamic_cast<BoardWidgetBackend *>(widget());
  connect(backend, &BoardWidgetBackend::moveMade, this, &BoardWidget::receiveMoveMade);
  connect(backend, &BoardWidgetBackend::whiteTimerTicked, this, &BoardWidget::receiveWhiteTimerTick);
  connect(backend, &BoardWidgetBackend::blackTimerTicked, this, &BoardWidget::receiveBlackTimerTick);
  connect(backend, &BoardWidgetBackend::evalBarUpdate, this, &BoardWidget::receiveEvalBarUpdate);
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

QString BoardWidget::formattedTime(int ms) const {
  if (ms < 0)
    return "";
  int sec = ms / 1000;
  ms %= 1000;
  int min = sec / 60;
  sec %= 60;
  int h = min / 60;
  min %= 60;
  if (min)
    return QStringLiteral("%1:%2:%3")
        .arg(QString::number(h), 2, '0')
        .arg(QString::number(min), 2, '0')
        .arg(QString::number(sec), 2, '0');
  else
    return QStringLiteral("%1:%2:%3.%4")
        .arg(QString::number(h), 2, '0')
        .arg(QString::number(min), 2, '0')
        .arg(QString::number(sec), 2, '0')
        .arg(QString::number(ms), 3, '0');
}

QString BoardWidget::fillPGNTag(const QSettings &settings, const QString &key) const {
  QString value = settings.value(key).toString();
  return value.isEmpty() ? "?" : value;
}
