#include "uciengine.h"
#include <QRegularExpression>
#include <QSignalSpy>
#include <QSettings>

UCIEngine::UCIEngine(const QString &path, bool white) : process(new QProcess) {
  QStringList args = path.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
  QString cmd = args.first();
  args.removeFirst();
  process->start(cmd, args);
  process->setReadChannel(QProcess::StandardOutput);

  sendCommand("uci");
  waitResponse("uciok");

  QSettings settings;
  QString side = white ? "whiteELO" : "blackELO";
  if (settings.value(side + "Enabled").toBool()) {
    QString elo = settings.value(side, 1800).toString();
    sendCommand("setoption name UCI_LimitStrength value true");
    sendCommand("setoption name UCI_Elo value " + elo);
  }

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
    if (!spy.wait(15000))
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
    if (!spy.wait(30000))
      throw UCIException();
    output = QString::fromUtf8(process->readAllStandardOutput()).split('\n');
    output.removeAll({});
    for (const QString &str : output)
      qDebug() << str;
    if (output.last().startsWith("bestmove")) {
      QString moveString = output.last().split(' ')[1];
      return moveString;
    }
  }
}
