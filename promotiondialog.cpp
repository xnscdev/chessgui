#include "promotiondialog.h"
#include "ui_PromotionDialog.h"

PromotionDialog::PromotionDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PromotionDialog) {
  ui->setupUi(this);
  setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

PromotionDialog::~PromotionDialog() {
  delete ui;
}
