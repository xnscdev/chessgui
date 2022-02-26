#include "chessgui.h"
#include "gamevariant.h"
#include <QApplication>

int main(int argc, char **argv) {
  QApplication a(argc, argv);
  QCoreApplication::setOrganizationName("XNSC");
  QCoreApplication::setOrganizationDomain("xnscdev.github.com");
  QCoreApplication::setApplicationName("ChessGUI");
  QCoreApplication::setApplicationVersion(PROJECT_VERSION);
  initGameData();
  ChessGUI w;
  w.show();
  return QApplication::exec();
}
