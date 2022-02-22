#include "chessgui.h"
#include "gamevariant.h"
#include <QApplication>

int main(int argc, char **argv) {
  QApplication a(argc, argv);
  initGameData();
  ChessGUI w;
  w.show();
  return QApplication::exec();
}
