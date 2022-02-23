#ifndef __POSITION_H
#define __POSITION_H

#include "piece.h"
#include <QList>
#include <QPoint>
#include <QHash>

#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
using qhash_result_t = size_t;
#else
using qhash_result_t = unsigned int;
#endif

typedef QList<QList<GamePiece>> GamePosition;

struct Move {
  QPoint from;
  QPoint to;
  QPoint capture;
  QPoint ep;

  operator QString() const;
  bool operator==(const Move &o) const { return from == o.from && to == o.to; }
  friend qhash_result_t qHash(const Move &move, qhash_result_t seed) { return qHashMulti(seed, move.from, move.to); }
};

QHash<QPoint, Move> availableMoves(const GamePosition &pos, QPoint ep, QPoint from);

#endif
