#include "chessgui.h"

#include <QApplication>

int main(int argc, char **argv) {
  QApplication a(argc, argv);
  ChessGUI w;
  w.show();
  return QApplication::exec();
}
