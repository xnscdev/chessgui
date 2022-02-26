#include "gamevariant.h"
#include "assets.h"

GameVariant *loadedVariant;
QSettings *pieceConfig;

QString GameVariant::moveName(GamePosition &position, const Move &move, QPoint ep, Piece *promote, bool white) const {
  if (promote)
    return move + notation[promote->name];
  else
    return move;
}

DefaultGameVariant::DefaultGameVariant()
    : rookPiece(new Piece("rook")), knightPiece(new Piece("knight")), bishopPiece(new Piece("bishop")),
      queenPiece(new Piece("queen")), kingPiece(new Piece("king")), pawnPiece(new Piece("pawn")) {
  size = QSize(8, 8);
  pieces["rook"] = rookPiece;
  pieces["knight"] = knightPiece;
  pieces["bishop"] = bishopPiece;
  pieces["queen"] = queenPiece;
  pieces["king"] = kingPiece;
  pieces["pawn"] = pawnPiece;
  notation["rook"] = "r";
  notation["knight"] = "n";
  notation["bishop"] = "b";
  notation["queen"] = "q";
  notation["king"] = "k";
}

void DefaultGameVariant::setup(GamePosition &position) {
  position[0][0] = GamePiece(rookPiece, true);
  position[0][1] = GamePiece(knightPiece, true);
  position[0][2] = GamePiece(bishopPiece, true);
  position[0][3] = GamePiece(queenPiece, true);
  position[0][4] = GamePiece(kingPiece, true);
  position[0][5] = GamePiece(bishopPiece, true);
  position[0][6] = GamePiece(knightPiece, true);
  position[0][7] = GamePiece(rookPiece, true);

  position[7][0] = GamePiece(rookPiece, false);
  position[7][1] = GamePiece(knightPiece, false);
  position[7][2] = GamePiece(bishopPiece, false);
  position[7][3] = GamePiece(queenPiece, false);
  position[7][4] = GamePiece(kingPiece, false);
  position[7][5] = GamePiece(bishopPiece, false);
  position[7][6] = GamePiece(knightPiece, false);
  position[7][7] = GamePiece(rookPiece, false);

  for (int i = 0; i < 8; i++) {
    position[1][i] = GamePiece(pawnPiece, true);
    position[6][i] = GamePiece(pawnPiece, false);
  }
}

QString DefaultGameVariant::moveName(GamePosition &position, const Move &move, QPoint ep, Piece *promote,
                                     bool white) const {
  Piece *piece = position[move.from.y()][move.from.x()].piece;
  QString str;
  if (move.castle.x() != -1) {
    if (move.castle.x() < move.from.x())
      return "O-O-O";
    else
      return "O-O";
  }
  if (piece != pawnPiece)
    str += notation[piece->name].toUpper();
  for (int y = 0; y < size.height(); y++) {
    bool found = false;
    for (int x = 0; x < size.width(); x++) {
      if (QPoint(x, y) != move.from && position[y][x].piece == piece && position[y][x].white == white) {
        QHash<QPoint, Move> moves = availableMoves(position, ep, {x, y});
        QMutableHashIterator<QPoint, Move> it(moves);
        while (it.hasNext()) {
          it.next();
          if (it.value().to == move.to && legalPosition(positionAfterMove(position, it.value()), white)) {
            if (x != move.from.x())
              str += static_cast<QChar>('a' + move.from.x());
            else
              str += QString::number(move.from.y() + 1);
            found = true;
            break;
          }
        }
      }
      if (found)
        break;
    }
    if (found)
      break;
  }

  if (position[move.to.y()][move.to.x()].piece || move.capture.x() != -1) {
    if (piece == pawnPiece)
      str += static_cast<QChar>('a' + move.from.x());
    str += 'x';
  }
  str += static_cast<QChar>('a' + move.to.x());
  str += QString::number(move.to.y() + 1);

  if (promote) {
    str += "=";
    str += notation[promote->name].toUpper();
  }

  GamePosition posCopy = positionAfterMove(position, move);
  if (!legalPosition(posCopy, !white)) {
    for (int y = 0; y < size.height(); y++) {
      for (int x = 0; x < size.width(); x++) {
        GamePiece &testPiece = posCopy[y][x];
        if (testPiece.piece && testPiece.white != white) {
          QHash<QPoint, Move> moves = availableMoves(posCopy, ep, {x, y});
          QHashIterator<QPoint, Move> it(moves);
          while (it.hasNext()) {
            it.next();
            if (legalPosition(positionAfterMove(posCopy, it.value()), !white)) {
              str += "+";
              return str;
            }
          }
        }
      }
    }
    str += "#";
  }
  return str;
}

void initGameData() {
  pieceConfig = loadSettings("pieces.ini");
  loadedVariant = new DefaultGameVariant;
}
