#include "chessgui.h"
#include "ui_chessgui.h"
#include "gamesetupdialog.h"
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

void ChessGUI::newGame() {
  ui->boardWidget->reset();
  ui->movesList->clearMoves();
}

void ChessGUI::saveGame() {
  QString contents = ui->boardWidget->metadataPGN();
  contents += ui->movesList->pgnMoveString + '\n';

  QString path = QFileDialog::getSaveFileName(this, "Save game", QDir::homePath(), "Portable Game Notation (.pgn)");
  if (path.isEmpty())
    return;
  QFile file(path);
  file.open(QIODevice::WriteOnly);
  file.write(contents.toUtf8());
  file.close();
}

void ChessGUI::setupGame() {
  GameSetupDialog dialog(this);
  dialog.exec();
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
