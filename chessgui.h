#ifndef CHESSGUI_H
#define CHESSGUI_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class ChessGUI;
}
QT_END_NAMESPACE

class ChessGUI : public QMainWindow {
  Q_OBJECT

public:
  explicit ChessGUI(QWidget *parent = nullptr);
  ~ChessGUI() override;

public slots:
  void newGame();
  void saveGame();

private:
  Ui::ChessGUI *ui;
};

#endif
