#ifndef __PROMOTIONDIALOG_H
#define __PROMOTIONDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class PromotionDialog;
}
QT_END_NAMESPACE

class PromotionDialog : public QDialog {
  Q_OBJECT

public:
  explicit PromotionDialog(QWidget *parent = nullptr);
  ~PromotionDialog() override;

private:
  Ui::PromotionDialog *ui;
};

#endif
