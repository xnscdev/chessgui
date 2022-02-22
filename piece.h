#ifndef __PIECE_H
#define __PIECE_H

#include <QImage>
#include <QObject>

class Piece {
public:
  enum class MovementType {
    Leaper,
    Rider,
    Hopper
  };

  enum class CaptureRule {
    CannotCapture,
    CanCapture,
    MustCapture
  };

  struct MovementRule {
    MovementType type;
    int dx;
    int dy;
    bool omnidirectional;
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

  GamePiece() : piece(nullptr), white(false) {}
  GamePiece(Piece *piece, bool white) : piece(piece), white(white) {}
};

#endif
