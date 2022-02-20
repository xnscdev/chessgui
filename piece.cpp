#include "piece.h"
#include <utility>

QRectF Piece::iconBox(0, 0, 128, 128);

Piece::Piece(QString name) : name(std::move(name)) {
  QString str = this->name.replace(' ', '-');
  whiteIcon = QImage(":/icons/white-" + str + ".png");
  blackIcon = QImage(":/icons/black-" + str + ".png");
}
