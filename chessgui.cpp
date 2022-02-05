#include "chessgui.h"
#include "ui_chessgui.h"

ChessGUI::ChessGUI(QWidget *parent) : QMainWindow(parent), ui(new Ui::ChessGUI) {
  ui->setupUi(this);
}

ChessGUI::~ChessGUI() {
  delete ui;
}
