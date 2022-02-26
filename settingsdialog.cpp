#include "settingsdialog.h"
#include "ui_SettingsDialog.h"
#include <QMessageBox>
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::SettingsDialog) {
  ui->setupUi(this);
  ui->helpButton->setIcon(QIcon(":/gui/icon-help.svg"));
  ui->addEngineButton->setIcon(QIcon(":/gui/icon-add.svg"));
  ui->removeEngineButton->setIcon(QIcon(":/gui/icon-remove.svg"));

  connect(ui->enginesList, &QListWidget::itemSelectionChanged, this, &SettingsDialog::selectionChanged);
  connect(ui->helpButton, &QPushButton::clicked, this, &SettingsDialog::showHelp);
  connect(ui->addEngineButton, &QPushButton::clicked, this, &SettingsDialog::addEngine);
  connect(ui->removeEngineButton, &QPushButton::clicked, this, &SettingsDialog::removeEngine);

  loadSettings();
}

SettingsDialog::~SettingsDialog() {
  delete ui;
}

void SettingsDialog::saveSettings() {
  settings.clear();
  settings.beginWriteArray("engines");
  for (int i = 0; i < ui->enginesList->count(); i++) {
    settings.setArrayIndex(i);
    settings.setValue("name", ui->enginesList->item(i)->text());
  }
  settings.endArray();
}

void SettingsDialog::loadSettings() {
  int size = settings.beginReadArray("engines");
  for (int i = 0; i < size; i++) {
    settings.setArrayIndex(i);
    addEngineCommand(settings.value("name").toString());
  }
  settings.endArray();
}

QListWidgetItem *SettingsDialog::addEngineCommand(const QString &cmd) {
  auto *item = new QListWidgetItem(cmd);
  item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
  ui->enginesList->insertItem(ui->enginesList->count(), item);
  return item;
}

void SettingsDialog::selectionChanged() {
  ui->removeEngineButton->setDisabled(ui->enginesList->selectedItems().isEmpty());
}

void SettingsDialog::showHelp() {
  QMessageBox box(this);
  box.setIcon(QMessageBox::Question);
  box.setText("Adding Engines");
  box.setInformativeText("Add chess engines to the list to make them available when setting up a game. "
                         "Each entry should first contain the path or command of the engine executable, followed "
                         "by any arguments to pass to the engine, separated by spaces.");
  box.exec();
}

void SettingsDialog::addEngine() {
  QListWidgetItem *item = addEngineCommand("");
  ui->enginesList->editItem(item);
}

void SettingsDialog::removeEngine() {
  if (ui->enginesList->selectedItems().size() == 1)
    ui->enginesList->takeItem(ui->enginesList->row(ui->enginesList->selectedItems()[0]));
}
