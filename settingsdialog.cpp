#include "settingsdialog.h"
#include "ui_SettingsDialog.h"
#include <QMessageBox>
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::SettingsDialog) {
  ui->setupUi(this);
  ui->helpButton->setIcon(QIcon(":/gui/icon-help.svg"));
  ui->addEngineButton->setIcon(QIcon(":/gui/icon-add.svg"));
  ui->removeEngineButton->setIcon(QIcon(":/gui/icon-remove.svg"));

  ui->enginesList->setColumnCount(2);
  ui->enginesList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->enginesList->setHorizontalHeaderLabels({"Name", "Command"});
  ui->enginesList->verticalHeader()->hide();

  connect(ui->enginesList, &QTableWidget::itemSelectionChanged, this, &SettingsDialog::selectionChanged);
  connect(ui->helpButton, &QPushButton::clicked, this, &SettingsDialog::showHelp);
  connect(ui->addEngineButton, &QPushButton::clicked, this, &SettingsDialog::addEngine);
  connect(ui->removeEngineButton, &QPushButton::clicked, this, &SettingsDialog::removeEngine);
  connect(ui->timeControlBox, &QCheckBox::clicked, ui->baseTimeBox, &QLineEdit::setEnabled);
  connect(ui->timeControlBox, &QCheckBox::clicked, ui->moveBonusBox, &QLineEdit::setEnabled);

  loadSettings();
}

SettingsDialog::~SettingsDialog() {
  delete ui;
}

void SettingsDialog::saveSettings() {
  settings.clear();

  settings.setValue("humanPlayer1Name", ui->humanPlayer1Name->text());
  settings.setValue("humanPlayer2Name", ui->humanPlayer2Name->text());
  settings.setValue("humanPlayer1Name", ui->humanPlayer1Name->text());
  settings.setValue("humanPlayer2Name", ui->humanPlayer2Name->text());
  settings.setValue("whitePlayer", ui->whitePlayer->currentIndex());
  settings.setValue("whiteELO", ui->whiteELO->value());
  settings.setValue("blackPlayer", ui->blackPlayer->currentIndex());
  settings.setValue("blackELO", ui->blackELO->value());
  settings.setValue("event", ui->eventBox->text());
  settings.setValue("site", ui->siteBox->text());
  settings.setValue("date", ui->gameDate->date());
  settings.setValue("round", ui->roundBox->text());
  settings.setValue("timeControl", ui->timeControlBox->isChecked());
  settings.setValue("baseTime", ui->baseTimeBox->value());
  settings.setValue("moveBonus", ui->moveBonusBox->value());

  settings.beginWriteArray("engines");
  for (int i = 0, j = 0; i < ui->enginesList->rowCount(); i++) {
    settings.setArrayIndex(j);
    if (ui->enginesList->item(i, 0)->text().isEmpty())
      continue;
    settings.setValue("name", ui->enginesList->item(i, 0)->text());
    settings.setValue("command", ui->enginesList->item(i, 1)->text());
    j++;
  }
  settings.endArray();
}

void SettingsDialog::loadSettings() {
  ui->humanPlayer1Name->setText(settings.value("humanPlayer1Name").toString());
  ui->humanPlayer2Name->setText(settings.value("humanPlayer2Name").toString());
  ui->whiteELOEnabled->setChecked(settings.value("whiteELOEnabled").toBool());
  ui->whiteELO->setValue(settings.value("whiteELO", 1800).toInt());
  ui->blackELOEnabled->setChecked(settings.value("blackELOEnabled").toBool());
  ui->blackELO->setValue(settings.value("blackELO", 1800).toInt());
  ui->eventBox->setText(settings.value("event").toString());
  ui->siteBox->setText(settings.value("site").toString());
  ui->gameDate->setDate(settings.value("date", QDate::currentDate()).toDate());
  ui->roundBox->setText(settings.value("round").toString());
  ui->timeControlBox->setChecked(settings.value("timeControl").toBool());
  ui->baseTimeBox->setValue(settings.value("baseTime", 5).toInt());
  ui->moveBonusBox->setValue(settings.value("moveBonus").toInt());

  ui->whiteELO->setEnabled(ui->whiteELOEnabled->isChecked());
  ui->blackELO->setEnabled(ui->blackELOEnabled->isChecked());
  ui->baseTimeBox->setEnabled(ui->timeControlBox->isChecked());
  ui->moveBonusBox->setEnabled(ui->timeControlBox->isChecked());

  int size = settings.beginReadArray("engines");
  ui->enginesList->setRowCount(size);
  for (int i = 0; i < size; i++) {
    settings.setArrayIndex(i);
    ui->enginesList->setItem(i, 0, new QTableWidgetItem(settings.value("name").toString()));
    ui->enginesList->setItem(i, 1, new QTableWidgetItem(settings.value("command").toString()));
  }
  settings.endArray();

  updatePlayerComboBoxes();
  ui->whitePlayer->setCurrentIndex(settings.value("whitePlayer", 0).toInt());
  ui->blackPlayer->setCurrentIndex(settings.value("blackPlayer", 1).toInt());
}

void SettingsDialog::updatePlayerComboBoxes() {
  ui->whitePlayer->clear();
  ui->whitePlayer->addItem("Human Player 1");
  ui->whitePlayer->addItem("Human Player 2");
  ui->blackPlayer->clear();
  ui->blackPlayer->addItem("Human Player 1");
  ui->blackPlayer->addItem("Human Player 2");

  for (int i = 0; i < ui->enginesList->rowCount(); i++) {
    QString engine = ui->enginesList->item(i, 0)->text() + " (" + ui->enginesList->item(i, 1)->text() + ")";
    ui->whitePlayer->addItem(engine);
    ui->blackPlayer->addItem(engine);
  }
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
  int row = ui->enginesList->rowCount();
  ui->enginesList->insertRow(row);
  ui->enginesList->edit(ui->enginesList->model()->index(row, 0));
}

void SettingsDialog::removeEngine() {
  for (auto &range : ui->enginesList->selectedRanges()) {
    for (int i = range.topRow(); i <= range.bottomRow(); i++)
      ui->enginesList->removeRow(range.topRow());
  }
  updatePlayerComboBoxes();
  ui->whitePlayer->setCurrentIndex(0);
  ui->blackPlayer->setCurrentIndex(1);
}
