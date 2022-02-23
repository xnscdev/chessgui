#ifndef __PIECE_H
#define __PIECE_H

#include <QImage>
#include <QObject>

class Piece {
public:
  enum class MovementType { Leaper, Slider, Rider, Hopper, EnPassant, Castle };

  enum class CaptureRule { CannotCapture, CanCapture, MustCapture };

  struct MovementRule {
    MovementType type;
    int dx;
    int dy;
    bool omnidirectional;
    bool firstMove;
    bool setEp;
    CaptureRule captures;
  };

  static QRectF iconBox;
  QString name;
  QList<MovementRule> moveRules;

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
  bool moved;

  GamePiece() : piece(nullptr), white(false), moved(false) {}
  GamePiece(Piece *piece, bool white) : piece(piece), white(white), moved(false) {}
};

#endif
