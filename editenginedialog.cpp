#include "editenginedialog.h"
#include "ui_editenginedialog.h"

EditEngineDialog::EditEngineDialog(QWidget *parent) : QDialog(parent), ui(new Ui::EditEngineDialog) {
  ui->setupUi(this);
}

EditEngineDialog::~EditEngineDialog() {
  delete ui;
}

QString EditEngineDialog::engineName() const {
  return ui->engineName->text();
}

QString EditEngineDialog::engineCommand() const {
  return ui->command->text();
}
