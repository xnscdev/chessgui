#include "chessgui.h"
#include "ui_chessgui.h"
#include <QFileDialog>

ChessGUI::ChessGUI(QWidget *parent) : QMainWindow(parent), ui(new Ui::ChessGUI) {
  ui->setupUi(this);
  connect(ui->boardWidget, &BoardWidget::moveMade, ui->movesList, &MovesListWidget::recordMove);
  connect(ui->firstMoveButton, &QPushButton::clicked, ui->boardWidget, &BoardWidget::toFirstMove);
  connect(ui->prevMoveButton, &QPushButton::clicked, ui->boardWidget, &BoardWidget::toPrevMove);
  connect(ui->nextMoveButton, &QPushButton::clicked, ui->boardWidget, &BoardWidget::toNextMove);
  connect(ui->lastMoveButton, &QPushButton::clicked, ui->boardWidget, &BoardWidget::toLastMove);
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
  QFile file(path);
  file.open(QIODevice::WriteOnly);
  file.write(contents.toUtf8());
  file.close();
}
