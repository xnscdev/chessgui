#ifndef __GAMEVARIANT_H
#define __GAMEVARIANT_H

#include "piece.h"
#include <QMap>
#include <QList>
#include <QSize>
#include <QSettings>

typedef QList<QList<GamePiece>> GamePosition;

class GameVariant {
public:
  QSize size;

  virtual void setup(GamePosition &position) = 0;
};

class DefaultGameVariant : public GameVariant {
public:
  DefaultGameVariant();
  void setup(GamePosition &position) override;

private:
  Piece *rookPiece;
  Piece *knightPiece;
  Piece *bishopPiece;
  Piece *queenPiece;
  Piece *kingPiece;
  Piece *pawnPiece;
};

extern GameVariant *loadedVariant;
extern QSettings *pieceConfig;

void initGameData();

#endif
