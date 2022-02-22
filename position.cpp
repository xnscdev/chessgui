#include "position.h"

static bool tryAddMove(const GamePosition &pos, const Piece::MovementRule &rule, QSet<Move> &moves, QPoint from, int dx,
                       int dy, qsizetype width, qsizetype height) {
  int x = from.x() + dx;
  int y = from.y() + dy;
  if (x >= 0 && x < width && y >= 0 && y < height &&
      (rule.captures == Piece::CaptureRule::CanCapture ||
       (rule.captures == Piece::CaptureRule::MustCapture) ==
           (pos[y][x].piece && pos[y][x].white != pos[from.y()][from.x()].white))) {
    moves.insert({from, {x, y}});
    return true;
  }
  return false;
}

Move::operator QString() const {
  QString str;
  str += static_cast<QChar>('a' + from.x());
  str += QString::number(from.y() + 1);
  str += static_cast<QChar>('a' + to.x());
  str += QString::number(to.y() + 1);
  return str;
}

GamePosition applyMove(GamePosition pos, const Move &move) {
  GamePiece &fromTile = pos[move.from.y()][move.from.x()];
  GamePiece &toTile = pos[move.to.y()][move.to.x()];
  toTile.piece = fromTile.piece;
  toTile.white = fromTile.white;
  fromTile.piece = nullptr;
  return pos;
}

QSet<Move> availableMoves(const GamePosition &pos, QPoint from) {
  QSet<Move> moves;
  qsizetype height = pos.size();
  qsizetype width = pos[0].size();
  Piece *piece = pos[from.y()][from.x()].piece;
  for (auto &rule : piece->moveRules) {
    switch (rule.type) {
    case Piece::MovementType::Leaper:
      tryAddMove(pos, rule, moves, from, rule.dx, rule.dy, width, height);
      if (rule.omnidirectional) {
        tryAddMove(pos, rule, moves, from, -rule.dx, rule.dy, width, height);
        tryAddMove(pos, rule, moves, from, rule.dx, -rule.dy, width, height);
        tryAddMove(pos, rule, moves, from, -rule.dx, -rule.dy, width, height);
      }
      break;
    case Piece::MovementType::Rider:
      for (int i = 1; tryAddMove(pos, rule, moves, from, rule.dx * i, rule.dy * i, width, height);)
        i++;
      if (rule.omnidirectional) {
        for (int i = 1; tryAddMove(pos, rule, moves, from, -rule.dx * i, rule.dy * i, width, height);)
          i++;
        for (int i = 1; tryAddMove(pos, rule, moves, from, rule.dx * i, -rule.dy * i, width, height);)
          i++;
        for (int i = 1; tryAddMove(pos, rule, moves, from, -rule.dx * i, -rule.dy * i, width, height);)
          i++;
      }
      break;
    case Piece::MovementType::Hopper:
      break; // TODO: Implement
    }
  }
  return moves;
}
