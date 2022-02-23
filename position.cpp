#include "position.h"

static bool tryAddMove(const GamePosition &pos, const Piece::MovementRule &rule, QHash<QPoint, Move> &moves, QPoint from,
                       int dx, int dy, qsizetype width, qsizetype height, QPoint capture = QPoint{-1, 0},
                       QPoint ep = QPoint{-1, 0}) {
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
  moves.insert({x, y}, {from, {x, y}, capture, ep});
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

Move::operator QString() const {
  QString str;
  str += static_cast<QChar>('a' + from.x());
  str += QString::number(from.y() + 1);
  str += static_cast<QChar>('a' + to.x());
  str += QString::number(to.y() + 1);
  return str;
}

QHash<QPoint, Move> availableMoves(const GamePosition &pos, QPoint ep, QPoint from) {
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
    }
  }
  return moves;
}
