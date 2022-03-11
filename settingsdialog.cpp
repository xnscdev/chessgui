#include "settingsdialog.h"
#include "editenginedialog.h"
#include "ui_settingsdialog.h"
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
  settings.setValue("whiteELOEnabled", ui->whiteELOEnabled->isChecked());
  settings.setValue("whiteELO", ui->whiteELO->value());
  settings.setValue("blackPlayer", ui->blackPlayer->currentIndex());
  settings.setValue("blackELOEnabled", ui->blackELOEnabled->isChecked());
  settings.setValue("blackELO", ui->blackELO->value());
  settings.setValue("event", ui->eventBox->text());
  settings.setValue("site", ui->siteBox->text());
  settings.setValue("date", ui->gameDate->date());
  settings.setValue("round", ui->roundBox->text());
  settings.setValue("timeControl", ui->timeControlBox->isChecked());
  settings.setValue("baseTime", ui->baseTimeBox->value());
  settings.setValue("moveBonus", ui->moveBonusBox->value());
  settings.setValue("evalEngine", ui->evalEngineBox->currentIndex());
  settings.setValue("engineDepth", ui->engineDepthBox->value());

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
  ui->engineDepthBox->setValue(settings.value("engineDepth", 25).toInt());

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

  ui->whitePlayer->clear();
  ui->whitePlayer->addItem("Human Player 1");
  ui->whitePlayer->addItem("Human Player 2");
  ui->blackPlayer->clear();
  ui->blackPlayer->addItem("Human Player 1");
  ui->blackPlayer->addItem("Human Player 2");
  ui->evalEngineBox->clear();
  ui->evalEngineBox->addItem("(None)");

  for (int i = 0; i < ui->enginesList->rowCount(); i++) {
    QString engine = ui->enginesList->item(i, 0)->text();
    ui->whitePlayer->addItem(engine);
    ui->blackPlayer->addItem(engine);
    ui->evalEngineBox->addItem(engine);
  }
  ui->whitePlayer->setCurrentIndex(settings.value("whitePlayer", 0).toInt());
  ui->blackPlayer->setCurrentIndex(settings.value("blackPlayer", 1).toInt());
  ui->evalEngineBox->setCurrentIndex(settings.value("evalEngine").toInt());
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
  EditEngineDialog dialog(this);
  if (dialog.exec() != QDialog::Accepted)
    return;
  QString name = dialog.engineName().trimmed();
  QString command = dialog.engineCommand().trimmed();
  if (name.isEmpty() || command.isEmpty())
    return;
  int row = ui->enginesList->rowCount();
  for (int i = 0; i < row; i++) {
    if (ui->enginesList->item(i, 0)->text() == name) {
      QMessageBox box(this);
      box.setIcon(QMessageBox::Critical);
      box.setText("Invalid engine name");
      box.setInformativeText("An engine already exists with the name \"" + name + "\"");
      box.exec();
      return;
    }
  }
  ui->enginesList->insertRow(row);
  ui->enginesList->setItem(row, 0, new QTableWidgetItem(name));
  ui->enginesList->setItem(row, 1, new QTableWidgetItem(command));
  ui->whitePlayer->addItem(name);
  ui->blackPlayer->addItem(name);
  ui->evalEngineBox->addItem(name);
}

void SettingsDialog::removeEngine() {
  QList<int> rows;
  for (auto &range : ui->enginesList->selectedRanges()) {
    for (int i = range.topRow(); i <= range.bottomRow(); i++)
      rows.append(i);
  }
  std::sort(rows.begin(), rows.end());
  if (rows.contains(ui->whitePlayer->currentIndex() - 2))
    ui->whitePlayer->setCurrentIndex(0);
  if (rows.contains(ui->blackPlayer->currentIndex() - 2))
    ui->blackPlayer->setCurrentIndex(1);
  if (rows.contains(ui->evalEngineBox->currentIndex()))
    ui->evalEngineBox->setCurrentIndex(-1);
  QListIterator<int> it(rows);
  it.toBack();
  while (it.hasPrevious()) {
    int row = it.previous();
    ui->enginesList->removeRow(row);
    ui->whitePlayer->removeItem(row + 2);
    ui->blackPlayer->removeItem(row + 2);
    ui->evalEngineBox->removeItem(row);
  }
}
