#ifndef __PIECE_H
#define __PIECE_H

#include <QPixmap>

class Piece {
public:
  QString name;
  QPixmap icon;

  explicit Piece(const QString &name);
};

#endif
