#include "chessgui.h"
#include "ui_chessgui.h"

ChessGUI::ChessGUI(QWidget *parent) : QMainWindow(parent), ui(new Ui::ChessGUI) {
  ui->setupUi(this);
  connect(ui->boardWidget, &BoardWidget::moveMade, ui->movesList, &MovesListWidget::recordMove);
}

ChessGUI::~ChessGUI() {
  delete ui;
}

void ChessGUI::newGame() {
  ui->boardWidget->reset();
  ui->movesList->clearMoves();
}

void ChessGUI::saveGame() {
  qDebug() << "Saving games is not implemented"; // TODO: Implement
}
