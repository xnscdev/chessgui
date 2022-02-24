#ifndef __PROMOTIONDIALOG_H
#define __PROMOTIONDIALOG_H

#include "gamevariant.h"
#include <QDialog>
#include <QTableWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
class PromotionDialog;
}
QT_END_NAMESPACE

class PromotionDialog : public QDialog {
  Q_OBJECT

public:
  Piece *selectedPiece;

  explicit PromotionDialog(Piece *oldPiece, GameVariant &game, bool white, QWidget *parent = nullptr);
  ~PromotionDialog() override;

private:
  class PieceIconItem : public QTableWidgetItem {
  public:
    Piece *piece;

    PieceIconItem(Piece *piece, bool white) : piece(piece), white(white) {}
    void setIcon();

  private:
    bool white;
  };

  static const int iconsPerRow = 5;
  Ui::PromotionDialog *ui;

private slots:
  void selectPiece(QTableWidgetItem *item) { selectedPiece = dynamic_cast<PieceIconItem *>(item)->piece; }
};

#endif
