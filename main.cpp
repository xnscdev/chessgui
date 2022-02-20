#include "chessgui.h"
#include "gamevariant.h"
#include <QApplication>

int main(int argc, char **argv) {
  QApplication a(argc, argv);
  initDefaultVariant();
  ChessGUI w;
  w.show();
  return QApplication::exec();
}
