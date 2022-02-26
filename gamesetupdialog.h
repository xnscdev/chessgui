#ifndef __GAMESETUPDIALOG_H
#define __GAMESETUPDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class GameSetupDialog;
}
QT_END_NAMESPACE

class GameSetupDialog : public QDialog {
  Q_OBJECT

public:
  explicit GameSetupDialog(QWidget *parent = nullptr);
  ~GameSetupDialog() override;

private:
  Ui::GameSetupDialog *ui;

private slots:
  void timeControlChanged(int state);
};

#endif
