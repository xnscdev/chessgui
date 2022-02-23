#ifndef __GAMEVARIANT_H
#define __GAMEVARIANT_H

#include "position.h"
#include <QMap>
#include <QSettings>
#include <QSize>

class GameVariant {
public:
  QSize size;
  QMap<QString, Piece *> pieces;

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
