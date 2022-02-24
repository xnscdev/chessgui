#include "chessgui.h"
#include "ui_chessgui.h"

ChessGUI::ChessGUI(QWidget *parent) : QMainWindow(parent), ui(new Ui::ChessGUI) {
  ui->setupUi(this);
  connect(ui->boardWidget, &BoardWidget::moveMade, ui->movesList, &MovesListWidget::recordMove);
}

ChessGUI::~ChessGUI() {
  delete ui;
}
