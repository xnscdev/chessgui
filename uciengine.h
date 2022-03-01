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
  UCIEngine(const QString &path, bool white, bool eval = false, int maxDepth = 0);
  ~UCIEngine() override { process->kill(); delete process; }
  void sendCommand(const QString &cmd);
  void waitResponse(const QString &response);
  void startEval(const QString &moves, bool white);
  QString waitMove();

private:
  QProcess *process;
  bool doingEval;
  bool evalWhite;
  int maxDepth;

signals:
  void evalBarUpdate(int cp, const QString &label);

private slots:
  void evalReady();
};

#endif
