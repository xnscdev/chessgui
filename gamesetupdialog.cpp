#include "gamesetupdialog.h"
#include "ui_GameSetupDialog.h"

GameSetupDialog::GameSetupDialog(QWidget *parent) : QDialog(parent), ui(new Ui::GameSetupDialog) {
  ui->setupUi(this);
  connect(ui->timeControlBox, &QCheckBox::stateChanged, this, &GameSetupDialog::timeControlChanged);
}

GameSetupDialog::~GameSetupDialog() {
  delete ui;
}

void GameSetupDialog::timeControlChanged(int state) {
  if (state == Qt::Checked) {
    ui->baseTime->setEnabled(true);
    ui->moveBonus->setEnabled(true);
  }
  else {
    ui->baseTime->setEnabled(false);
    ui->moveBonus->setEnabled(false);
  }
}
