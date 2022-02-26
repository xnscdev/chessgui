#ifndef __SETTINGSDIALOG_H
#define __SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui {
class SettingsDialog;
}
QT_END_NAMESPACE

class SettingsDialog : public QDialog {
  Q_OBJECT

public:
  explicit SettingsDialog(QWidget *parent = nullptr);
  ~SettingsDialog() override;
  void saveSettings();

private:
  Ui::SettingsDialog *ui;
  QSettings settings;

  void loadSettings();
  QListWidgetItem *addEngineCommand(const QString &cmd);

private slots:
  void selectionChanged();
  void showHelp();
  void addEngine();
  void removeEngine();
};

#endif
