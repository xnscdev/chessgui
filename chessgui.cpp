#include "chessgui.h"
#include "settingsdialog.h"
#include "ui_chessgui.h"
#include <QFileDialog>

ChessGUI::ChessGUI(QWidget *parent) : QMainWindow(parent), ui(new Ui::ChessGUI) {
  ui->setupUi(this);
  connect(ui->boardWidget, &BoardWidget::moveMade, ui->movesList, &MovesListWidget::recordMove);
  connect(ui->boardWidget, &BoardWidget::moveMade, this, &ChessGUI::updateSelectedAfterMove);
  connect(ui->firstMoveButton, &QPushButton::clicked, this, &ChessGUI::toFirstMove);
  connect(ui->prevMoveButton, &QPushButton::clicked, this, &ChessGUI::toPrevMove);
  connect(ui->nextMoveButton, &QPushButton::clicked, this, &ChessGUI::toNextMove);
  connect(ui->lastMoveButton, &QPushButton::clicked, this, &ChessGUI::toLastMove);
  connect(ui->movesList, &QTableWidget::itemSelectionChanged, this, &ChessGUI::moveListSelected);
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
  ui->playerTimeLabel->setText(ui->boardWidget->formattedTime(!ui->boardWidget->reversed()));
  ui->opponentTimeLabel->setText(ui->boardWidget->formattedTime(ui->boardWidget->reversed()));
}

void ChessGUI::newGame() {
  ui->boardWidget->reset();
  ui->movesList->clearMoves();
  updatePlayerNames();
  updateTimers();
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
  ui->movesList->updateSelected(move + 1);
}

void ChessGUI::moveListSelected() {
  if (ui->movesList->selectedRanges().size() != 1)
    return;
  QTableWidgetSelectionRange selection = ui->movesList->selectedRanges()[0];
  ui->boardWidget->toMove(selection.topRow() * 2 + selection.leftColumn());
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
