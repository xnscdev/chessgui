#include "uciengine.h"
#include <QRegularExpression>
#include <QSignalSpy>

UCIEngine::UCIEngine(const QString &path) : process(new QProcess(this)) {
  QStringList args = path.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
  QString cmd = args.first();
  args.removeFirst();
  process->start(cmd, args);
  process->setReadChannel(QProcess::StandardOutput);

  sendCommand("uci");
  waitResponse("uciok");
  sendCommand("ucinewgame");
  sendCommand("isready");
  waitResponse("readyok");
}

void UCIEngine::sendCommand(const QString &cmd) {
  qDebug() << ">>" << cmd;
  process->write((cmd + '\n').toUtf8());
}

void UCIEngine::waitResponse(const QString &response) {
  while (true) {
    QSignalSpy spy(process, &QProcess::readyReadStandardOutput);
    if (!spy.wait())
      throw UCIException();
    output = QString::fromUtf8(process->readAllStandardOutput()).split('\n');
    output.removeAll({});
    for (const QString &str : output)
      qDebug() << str;
    if (output.last() == response)
      break;
  }
}

QString UCIEngine::waitMove() {
  while (true) {
    QSignalSpy spy(process, &QProcess::readyReadStandardOutput);
    if (!spy.wait())
      throw UCIException();
    output = QString::fromUtf8(process->readAllStandardOutput()).split('\n');
    output.removeAll({});
    for (const QString &str : output)
      qDebug() << str;
    if (output.last().startsWith("bestmove")) {
      QString moveString = output.last().split(' ')[1];
      qDebug() << "! Move:" << moveString;
      return moveString;
    }
  }
}
