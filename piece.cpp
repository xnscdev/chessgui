#include "piece.h"
#include "assets.h"
#include "gamevariant.h"
#include <utility>

QRectF Piece::iconBox(0, 0, 128, 128);

Piece::Piece(QString name) : name(std::move(name)) {
  QString str = this->name.replace(' ', '-');
  whiteIcon = loadImage("icons/white-" + str + ".png");
  blackIcon = loadImage("icons/black-" + str + ".png");
  int size = pieceConfig->beginReadArray(str);
  for (int i = 0; i < size; i++) {
    pieceConfig->setArrayIndex(i);
    MovementRule rule;
    rule.type = static_cast<MovementType>(pieceConfig->value("type").toInt());
    rule.dx = pieceConfig->value("dx").toInt();
    rule.dy = pieceConfig->value("dy").toInt();
    rule.omnidirectional = pieceConfig->value("omnidirectional").toBool();
    rule.firstMove = pieceConfig->value("firstMove").toBool();
    rule.captures = static_cast<CaptureRule>(pieceConfig->value("captures").toInt());
    moveRules.append(rule);
  }
  pieceConfig->endArray();
}
