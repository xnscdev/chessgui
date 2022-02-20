#ifndef __PIECE_H
#define __PIECE_H

#include <QImage>

class Piece {
public:
  static QRectF iconBox;
  QString name;

  explicit Piece(QString name);
  [[nodiscard]] const QImage &icon(bool white) const { return white ? whiteIcon : blackIcon; }

private:
  QImage whiteIcon;
  QImage blackIcon;
};

class GamePiece {
public:
  Piece *piece;
  bool white;

  GamePiece() : piece(nullptr), white(false) {}
  GamePiece(Piece *piece, bool white) : piece(piece), white(white) {}
};

#endif
