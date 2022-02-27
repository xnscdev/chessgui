#include "position.h"
#include <QHashIterator>

static bool tryAddMove(const GamePosition &pos, const Piece::MovementRule &rule, QHash<QPoint, Move> &moves,
                       QPoint from, int dx, int dy, qsizetype width, qsizetype height, QPoint capture = QPoint{-1, 0},
                       QPoint ep = QPoint{-1, 0}, QPoint castle = QPoint{-1, 0}) {
  bool white = pos[from.y()][from.x()].white;
  int x = from.x() + dx;
  int y = from.y() + (white ? dy : -dy);
  if (x < 0 || x >= width || y < 0 || y >= height)
    return false;
  if (pos[y][x].piece) {
    if (pos[y][x].white == white)
      return false;
    if (rule.captures == Piece::CaptureRule::CannotCapture)
      return false;
  }
  else if (rule.captures == Piece::CaptureRule::MustCapture)
    return false;
  moves.insert({x, y}, {from, {x, y}, capture, ep, castle});
  return true;
}

static void addSliderMove(const GamePosition &pos, const Piece::MovementRule &rule, QHash<QPoint, Move> &moves,
                          QPoint from, int dx, int dy, qsizetype width, qsizetype height) {
  bool white = pos[from.y()][from.x()].white;
  int sdy = dy;
  if (!white)
    dy = -dy;
  if (dx > 0) {
    for (int i = 1; i < dx; i++) {
      if (from.x() + i >= width || pos[from.y()][from.x() + i].piece)
        return;
    }
  }
  else if (dx < 0) {
    for (int i = 1; i < -dx; i++) {
      if (from.x() - i < 0 || pos[from.y()][from.x() - i].piece)
        return;
    }
  }
  else if (dy > 0) {
    for (int i = 1; i < dy; i++) {
      if (from.y() + i >= height || pos[from.y() + i][from.x()].piece)
        return;
    }
  }
  else if (dy < 0) {
    for (int i = 1; i < -dy; i++) {
      if (from.y() - i < 0 || pos[from.y() - i][from.x()].piece)
        return;
    }
  }
  dy = sdy;
  QPoint ep = {-1, 0};
  if (rule.setEp)
    ep = {from.x(), from.y() + (white ? dy - 1 : -dy + 1)};
  tryAddMove(pos, rule, moves, from, dx, dy, width, height, {-1, 0}, ep);
}

static void addRiderMoves(const GamePosition &pos, const Piece::MovementRule &rule, QHash<QPoint, Move> &moves,
                          QPoint from, int dx, int dy, qsizetype width, qsizetype height) {
  bool white = pos[from.y()][from.x()].white;
  for (int i = 1; tryAddMove(pos, rule, moves, from, dx * i, dy * i, width, height); i++) {
    int x = from.x() + dx * i;
    int y = from.y() + (white ? dy : -dy) * i;
    if (pos[y][x].piece && pos[y][x].white != white)
      break;
  }
}

static void addEnPassant(const GamePosition &pos, const Piece::MovementRule &rule, QHash<QPoint, Move> &moves,
                         QPoint ep, QPoint from, int dx, int dy, qsizetype width, qsizetype height) {
  bool white = pos[from.y()][from.x()].white;
  int x = from.x() + dx;
  int y = from.y() + (white ? dy : -dy);
  if (x < 0 || x >= width || y < 0 || y >= height)
    return;
  if (QPoint(x, y) != ep)
    return;
  tryAddMove(pos, rule, moves, from, dx, dy, width, height, {x, from.y()});
}

static void addCastle(const GamePosition &pos, const Piece::MovementRule &rule, QHash<QPoint, Move> &moves, QPoint from,
                      int dx, qsizetype width, qsizetype height) {
  bool white = pos[from.y()][from.x()].white;
  if (pos[from.y()][from.x()].moved)
    return;
  if (!legalPosition(pos, white, false))
    return;
  GamePosition posCopy = pos;
  posCopy[from.y()][from.x()].piece = nullptr;
  if (dx < 0) {
    for (int i = from.x() - 1; i >= 0; i--) {
      if (pos[from.y()][i].piece) {
        if (pos[from.y()][i].piece->name != "rook" || pos[from.y()][i].white != white || pos[from.y()][i].moved)
          return;
        posCopy[from.y()][i].piece = pos[from.y()][from.x()].piece;
        posCopy[from.y()][i].white = white;
        if (!legalPosition(posCopy, white, false))
          return;
        tryAddMove(pos, rule, moves, from, dx, 0, width, height, {-1, 0}, {from.x() + dx + 1, from.y()}, {i, from.y()});
        break;
      }
    }
  }
  else if (dx > 0) {
    for (int i = from.x() + 1; i < width; i++) {
      if (pos[from.y()][i].piece) {
        if (pos[from.y()][i].piece->name != "rook" || pos[from.y()][i].white != white || pos[from.y()][i].moved)
          return;
        posCopy[from.y()][i].piece = pos[from.y()][from.x()].piece;
        posCopy[from.y()][i].white = white;
        if (!legalPosition(posCopy, white))
          return;
        tryAddMove(pos, rule, moves, from, dx, 0, width, height, {-1, 0}, {from.x() + dx - 1, from.y()}, {i, from.y()});
        break;
      }
    }
  }
}

Move::operator QString() const {
  QString str;
  str += static_cast<QChar>('a' + from.x());
  str += QString::number(from.y() + 1);
  str += static_cast<QChar>('a' + to.x());
  str += QString::number(to.y() + 1);
  str += promote;
  return str;
}

QHash<QPoint, Move> availableMoves(const GamePosition &pos, QPoint ep, QPoint from, bool checkCastle) {
  QHash<QPoint, Move> moves;
  qsizetype height = pos.size();
  qsizetype width = pos[0].size();
  Piece *piece = pos[from.y()][from.x()].piece;
  for (auto &rule : piece->moveRules) {
    if (rule.firstMove && pos[from.y()][from.x()].moved)
      continue;
    if (rule.type == Piece::MovementType::Slider && rule.dx && rule.dy)
      rule.type = Piece::MovementType::Leaper;
    switch (rule.type) {
    case Piece::MovementType::Leaper:
      tryAddMove(pos, rule, moves, from, rule.dx, rule.dy, width, height);
      if (rule.omnidirectional) {
        tryAddMove(pos, rule, moves, from, -rule.dx, rule.dy, width, height);
        tryAddMove(pos, rule, moves, from, rule.dx, -rule.dy, width, height);
        tryAddMove(pos, rule, moves, from, -rule.dx, -rule.dy, width, height);
      }
      break;
    case Piece::MovementType::Slider:
      addSliderMove(pos, rule, moves, from, rule.dx, rule.dy, width, height);
      if (rule.omnidirectional) {
        addSliderMove(pos, rule, moves, from, -rule.dx, rule.dy, width, height);
        addSliderMove(pos, rule, moves, from, rule.dx, -rule.dy, width, height);
        addSliderMove(pos, rule, moves, from, -rule.dx, -rule.dy, width, height);
      }
      break;
    case Piece::MovementType::Rider:
      addRiderMoves(pos, rule, moves, from, rule.dx, rule.dy, width, height);
      if (rule.omnidirectional) {
        addRiderMoves(pos, rule, moves, from, -rule.dx, rule.dy, width, height);
        addRiderMoves(pos, rule, moves, from, rule.dx, -rule.dy, width, height);
        addRiderMoves(pos, rule, moves, from, -rule.dx, -rule.dy, width, height);
      }
      break;
    case Piece::MovementType::Hopper:
      break; // TODO: Implement
    case Piece::MovementType::EnPassant:
      addEnPassant(pos, rule, moves, ep, from, rule.dx, rule.dy, width, height);
      break;
    case Piece::MovementType::Castle:
      if (checkCastle) {
        addCastle(pos, rule, moves, from, rule.dx, width, height);
        if (rule.omnidirectional)
          addCastle(pos, rule, moves, from, -rule.dx, width, height);
      }
      break;
    }
  }
  return moves;
}

GamePosition positionAfterMove(GamePosition position, const Move &move) {
  GamePiece &fromPiece = position[move.from.y()][move.from.x()];
  GamePiece &toPiece = position[move.to.y()][move.to.x()];
  toPiece.piece = fromPiece.piece;
  toPiece.white = fromPiece.white;
  toPiece.moved = true;
  fromPiece.piece = nullptr;
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
  return position;
}

bool legalPosition(const GamePosition &pos, bool white, bool checkCastle) {
  for (int y = 0; y < pos.size(); y++) {
    for (int x = 0; x < pos[y].size(); x++) {
      if (pos[y][x].piece && pos[y][x].white != white) {
        QHashIterator<QPoint, Move> it(availableMoves(pos, {-1, 0}, {x, y}, checkCastle));
        while (it.hasNext()) {
          it.next();
          const GamePiece &target = pos[it.value().to.y()][it.value().to.x()];
          if (target.piece && target.white == white && target.piece->royal)
            return false;
        }
      }
    }
  }
  return true;
}
