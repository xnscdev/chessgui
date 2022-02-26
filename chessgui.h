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
  void setupGame();

private:
  Ui::ChessGUI *ui;

private slots:
  void updateSelectedAfterMove();
  void moveListSelected();
  void toFirstMove();
  void toPrevMove();
  void toNextMove();
  void toLastMove();
};

#endif
