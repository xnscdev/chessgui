#ifndef __UCIENGINE_H
#define __UCIENGINE_H

#include <QException>
#include <QProcess>

class UCIException : public QException {
public:
  void raise() const override { throw *this; }
  [[nodiscard]] UCIException *clone() const override { return new UCIException(*this); }
};

class UCIEngine : public QObject {
  Q_OBJECT

public:
  explicit UCIEngine(const QString &path);
  void sendCommand(const QString &cmd);
  void waitResponse(const QString &response);
  QString waitMove();

private:
  QProcess *process;
  QStringList output;
};

#endif
