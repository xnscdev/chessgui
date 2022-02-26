#ifndef __UCIENGINE_H
#define __UCIENGINE_H

#include <QException>
#include <QProcess>

class UCIException : public QException {
public:
  void raise() const override { throw *this; }
  [[nodiscard]] UCIException *clone() const override { return new UCIException(*this); }
};

class UCIEngine {
public:
  explicit UCIEngine(const QString &path);
  QStringList sendCommand(const QString &cmd);

private:
  QProcess process;
};

#endif
