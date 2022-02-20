#ifndef __GAMEVARIANT_H
#define __GAMEVARIANT_H

#include "piece.h"
#include <QMap>
#include <QList>
#include <QSize>

class GameVariant {
public:
  QSize size;
  QMap<QString, Piece> pieces;

  virtual void setup(QList<QList<GamePiece>> &position) = 0;
};

class DefaultGameVariant : public GameVariant {
public:
  DefaultGameVariant();
  void setup(QList<QList<GamePiece>> &position) override;

private:
  Piece rookPiece;
  Piece knightPiece;
  Piece bishopPiece;
  Piece queenPiece;
  Piece kingPiece;
  Piece pawnPiece;
};

extern GameVariant *loadedVariant;

void initDefaultVariant();

#endif
