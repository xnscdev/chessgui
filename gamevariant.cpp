#include "gamevariant.h"
#include "assets.h"

GameVariant *loadedVariant;
QSettings *pieceConfig;

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

void initGameData() {
  pieceConfig = loadSettings("pieces.ini");
  loadedVariant = new DefaultGameVariant;
}
