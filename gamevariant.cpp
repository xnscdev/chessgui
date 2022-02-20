#include "gamevariant.h"

GameVariant *loadedVariant;

DefaultGameVariant::DefaultGameVariant()
    : rookPiece("rook"), knightPiece("knight"), bishopPiece("bishop"), queenPiece("queen"), kingPiece("king"),
      pawnPiece("pawn") {
  size = QSize(8, 8);
  pieces.insert("rook", rookPiece);
  pieces.insert("knight", knightPiece);
  pieces.insert("bishop", bishopPiece);
  pieces.insert("queen", queenPiece);
  pieces.insert("king", kingPiece);
  pieces.insert("pawn", pawnPiece);
}

void DefaultGameVariant::setup(QList<QList<GamePiece>> &position) {
  position[0][0] = GamePiece(&rookPiece, true);
  position[0][1] = GamePiece(&knightPiece, true);
  position[0][2] = GamePiece(&bishopPiece, true);
  position[0][3] = GamePiece(&queenPiece, true);
  position[0][4] = GamePiece(&kingPiece, true);
  position[0][5] = GamePiece(&bishopPiece, true);
  position[0][6] = GamePiece(&knightPiece, true);
  position[0][7] = GamePiece(&rookPiece, true);

  position[7][0] = GamePiece(&rookPiece, false);
  position[7][1] = GamePiece(&knightPiece, false);
  position[7][2] = GamePiece(&bishopPiece, false);
  position[7][3] = GamePiece(&queenPiece, false);
  position[7][4] = GamePiece(&kingPiece, false);
  position[7][5] = GamePiece(&bishopPiece, false);
  position[7][6] = GamePiece(&knightPiece, false);
  position[7][7] = GamePiece(&rookPiece, false);

  for (int i = 0; i < 8; i++) {
    position[1][i] = GamePiece(&pawnPiece, true);
    position[6][i] = GamePiece(&pawnPiece, false);
  }
}

void initDefaultVariant() {
  loadedVariant = new DefaultGameVariant;
}
