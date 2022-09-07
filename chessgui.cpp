#include "chessgui.h"
#include "settingsdialog.h"
#include "ui_chessgui.h"
#include <QFileDialog>

QString ChessGUI::evalBarStyle = "QProgressBar {\n"
                                 "background-color: black;\n"
                                 "border-radius: 7px;\n"
                                 "}\n"
                                 "\n"
                                 "QProgressBar::chunk {\n"
                                 "background-color: white;\n"
                                 "border-bottom-left-radius: 7px;\n"
                                 "border-bottom-right-radius: 7px;\n"
                                 "}";

QString ChessGUI::evalBarFullStyle = "QProgressBar {\n"
                                     "background-color: black;\n"
                                     "border-radius: 7px;\n"
                                     "}\n"
                                     "\n"
                                     "QProgressBar::chunk {\n"
                                     "background-color: white;\n"
                                     "border-radius: 7px;\n"
                                     "}";

ChessGUI::ChessGUI(QWidget *parent) : QMainWindow(parent), ui(new Ui::ChessGUI) {
  ui->setupUi(this);
  connect(ui->boardWidget, &BoardWidget::moveMade, ui->movesList, &MovesListWidget::recordMove);
  connect(ui->boardWidget, &BoardWidget::moveMade, this, &ChessGUI::updateSelectedAfterMove);
  connect(ui->boardWidget, &BoardWidget::whiteTimerTicked, this, &ChessGUI::updateWhiteTimer);
  connect(ui->boardWidget, &BoardWidget::blackTimerTicked, this, &ChessGUI::updateBlackTimer);
  connect(ui->boardWidget, &BoardWidget::evalBarUpdate, this, &ChessGUI::updateEvalBar);
  connect(ui->flipButton, &QPushButton::clicked, this, &ChessGUI::flipBoard);
  connect(ui->firstMoveButton, &QPushButton::clicked, this, &ChessGUI::toFirstMove);
  connect(ui->prevMoveButton, &QPushButton::clicked, this, &ChessGUI::toPrevMove);
  connect(ui->nextMoveButton, &QPushButton::clicked, this, &ChessGUI::toNextMove);
  connect(ui->lastMoveButton, &QPushButton::clicked, this, &ChessGUI::toLastMove);
  connect(ui->movesList, &QTableWidget::itemSelectionChanged, this, &ChessGUI::moveListSelected);
  ui->evalBar->setStyleSheet(evalBarStyle);
}

ChessGUI::~ChessGUI() {
  delete ui;
}

void ChessGUI::updatePlayerNames() {
  QSettings settings;
  if (ui->boardWidget->reversed()) {
    ui->playerLabel->setText(BoardWidget::playerName(settings, "blackPlayer", 1));
    ui->opponentLabel->setText(BoardWidget::playerName(settings, "whitePlayer", 0));
  }
  else {
    ui->playerLabel->setText(BoardWidget::playerName(settings, "whitePlayer", 0));
    ui->opponentLabel->setText(BoardWidget::playerName(settings, "blackPlayer", 1));
  }
}

void ChessGUI::updateTimers() {
  QSettings settings;
  if (settings.value("timeControl").toBool()) {
    int wms = settings.value("whiteBaseTime").toInt() * 60000;
    int bms = settings.value("blackBaseTime").toInt() * 60000;
    if (ui->boardWidget->reversed()) {
      ui->playerTimeLabel->setText(ui->boardWidget->formattedTime(bms));
      ui->opponentTimeLabel->setText(ui->boardWidget->formattedTime(wms));
    }
    else {
      ui->playerTimeLabel->setText(ui->boardWidget->formattedTime(wms));
      ui->opponentTimeLabel->setText(ui->boardWidget->formattedTime(bms));
    }
  }
}

void ChessGUI::closeEvent(QCloseEvent *event) {
  ui->boardWidget->closeEngines();
}

void ChessGUI::newGame() {
  updatePlayerNames();
  updateTimers();
  ui->movesList->clearMoves();
  ui->boardWidget->newGame();
}

void ChessGUI::saveGame() {
  QString contents = ui->boardWidget->metadataPGN();
  QString moves = ui->movesList->pgnMoveString;
  if (moves.contains('#'))
    moves = moves.mid(0, moves.indexOf("#") + 1);
  contents += moves + '\n';

  QString path = QFileDialog::getSaveFileName(this, "Save game", QDir::homePath(), "Portable Game Notation (.pgn)");
  if (path.isEmpty())
    return;
  QFile file(path);
  file.open(QIODevice::WriteOnly);
  file.write(contents.toUtf8());
  file.close();
}

void ChessGUI::openSettings() {
  SettingsDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted)
    dialog.saveSettings();
}

void ChessGUI::updateSelectedAfterMove() {
  int move = ui->boardWidget->historyMove();
  if (move == ui->boardWidget->history().size() - 1)
    ui->movesList->updateSelected(move);
}

void ChessGUI::moveListSelected() {
  if (ui->movesList->selectedRanges().size() != 1)
    return;
  QTableWidgetSelectionRange selection = ui->movesList->selectedRanges()[0];
  ui->boardWidget->toMove(selection.topRow() * 2 + selection.leftColumn());
}

void ChessGUI::flipBoard() {
  ui->boardWidget->flipBoard();
  ui->boardWidget->update();
  if (ui->boardWidget->gameRunning())
    updatePlayerNames();
}

void ChessGUI::toFirstMove() {
  ui->boardWidget->toFirstMove();
  ui->movesList->updateSelected(ui->boardWidget->historyMove());
}

void ChessGUI::toPrevMove() {
  ui->boardWidget->toPrevMove();
  ui->movesList->updateSelected(ui->boardWidget->historyMove());
}

void ChessGUI::toNextMove() {
  ui->boardWidget->toNextMove();
  ui->movesList->updateSelected(ui->boardWidget->historyMove());
}

void ChessGUI::toLastMove() {
  ui->boardWidget->toLastMove();
  ui->movesList->updateSelected(ui->boardWidget->historyMove());
}

void ChessGUI::updateWhiteTimer(const QString &time) {
  if (ui->boardWidget->reversed())
    ui->opponentTimeLabel->setText(time);
  else
    ui->playerTimeLabel->setText(time);
}

void ChessGUI::updateBlackTimer(const QString &time) {
  if (ui->boardWidget->reversed())
    ui->playerTimeLabel->setText(time);
  else
    ui->opponentTimeLabel->setText(time);
}

void ChessGUI::updateEvalBar(int cp, const QString &label) {
  if (cp >= 1000)
    ui->evalBar->setStyleSheet(evalBarFullStyle);
  else
    ui->evalBar->setStyleSheet(evalBarStyle);
  ui->evalBar->setValue(cp);
  ui->evalBarLabel->setText(label);
}
