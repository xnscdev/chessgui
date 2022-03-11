#include "promotiondialog.h"
#include "ui_promotiondialog.h"

PromotionDialog::PromotionDialog(Piece *oldPiece, GameVariant &game, bool white, QWidget *parent)
    : QDialog(parent), ui(new Ui::PromotionDialog), selectedPiece(nullptr) {
  ui->setupUi(this);
  setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

  ui->pieceIconTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->pieceIconTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->pieceIconTable->horizontalHeader()->hide();
  ui->pieceIconTable->verticalHeader()->hide();

  QMapIterator<QString, Piece *> it(game.pieces);
  int i = 0;
  while (it.hasNext()) {
    it.next();
    if (!it.value()->royal && it.value() != oldPiece)
      i++;
  }
  ui->pieceIconTable->setRowCount(static_cast<int>(i + iconsPerRow - 1) / iconsPerRow);
  ui->pieceIconTable->setColumnCount(iconsPerRow);

  it.toFront();
  i = 0;
  while (it.hasNext()) {
    it.next();
    if (it.value()->royal || it.value() == oldPiece)
      continue;
    auto *item = new PieceIconItem(it.value(), white);
    item->setIcon();
    ui->pieceIconTable->setItem(i / iconsPerRow, i % iconsPerRow, item);
    i++;
  }
  ui->pieceIconTable->resizeColumnsToContents();
  ui->pieceIconTable->resizeRowsToContents();

  connect(ui->pieceIconTable, &QTableWidget::itemClicked, this, &QDialog::accept);
  connect(ui->pieceIconTable, &QTableWidget::itemClicked, this, &PromotionDialog::selectPiece);
}

PromotionDialog::~PromotionDialog() {
  delete ui;
}

void PromotionDialog::PieceIconItem::setIcon() {
  setData(Qt::DecorationRole, QPixmap::fromImage(piece->icon(white).scaled(60, 60)));
}
