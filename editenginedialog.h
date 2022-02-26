#ifndef __EDITENGINEDIALOG_H
#define __EDITENGINEDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class EditEngineDialog;
}
QT_END_NAMESPACE

class EditEngineDialog : public QDialog {
  Q_OBJECT

public:
  explicit EditEngineDialog(QWidget *parent = nullptr);
  ~EditEngineDialog() override;
  QString engineName() const;
  QString engineCommand() const;

private:
  Ui::EditEngineDialog *ui;
};

#endif
