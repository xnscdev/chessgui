#include "uciengine.h"
#include <QRegularExpression>
#include <QSignalSpy>
#include <QSettings>

UCIEngine::UCIEngine(const QString &path, bool white, bool eval, int maxDepth) : process(new QProcess), maxDepth(maxDepth) {
  QStringList args = path.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
  QString cmd = args.first();
  args.removeFirst();
  process->start(cmd, args);
  process->setReadChannel(QProcess::StandardOutput);

  sendCommand("uci");
  waitResponse("uciok");

  doingEval = false;
  if (eval) {
    sendCommand("setoption name UCI_AnalyseMode value true");
    connect(process, &QProcess::readyReadStandardOutput, this, &UCIEngine::evalReady);
  }
  else {
    QSettings settings;
    QString side = white ? "whiteELO" : "blackELO";
    if (settings.value(side + "Enabled").toBool()) {
      QString elo = settings.value(side, 1800).toString();
      sendCommand("setoption name UCI_LimitStrength value true");
      sendCommand("setoption name UCI_Elo value " + elo);
    }
  }

  sendCommand("ucinewgame");
  sendCommand("isready");
  waitResponse("readyok");
}

void UCIEngine::sendCommand(const QString &cmd) {
  process->write((cmd + '\n').toUtf8());
}

void UCIEngine::waitResponse(const QString &response) {
  while (true) {
    QSignalSpy spy(process, &QProcess::readyReadStandardOutput);
    if (!spy.wait(15000))
      continue;
    QStringList output = QString::fromUtf8(process->readAllStandardOutput()).split(QRegularExpression{R"-((\r\n?|\n))-"});
    output.removeAll({});
    if (!output.isEmpty() && output.last() == response)
      break;
  }
}

void UCIEngine::startEval(const QString &moves, bool white) {
  doingEval = true;
  evalWhite = white;
  sendCommand("stop");
  sendCommand("position startpos moves" + moves);
  sendCommand("go infinite");
}

QString UCIEngine::waitMove() {
  while (true) {
    QSignalSpy spy(process, &QProcess::readyReadStandardOutput);
    if (!spy.wait(30000))
      continue;
    QStringList output = QString::fromUtf8(process->readAllStandardOutput()).split(QRegularExpression{R"-((\r\n?|\n))-"});
    output.removeAll({});
    if (output.isEmpty())
      continue;
    if (output.last().startsWith("bestmove")) {
      QString moveString = output.last().split(' ')[1];
      return moveString;
    }
  }
}

void UCIEngine::evalReady() {
  if (!doingEval)
    return;
  QStringList output = QString::fromUtf8(process->readAllStandardOutput()).split(QRegularExpression{R"-((\r\n?|\n))-"});
  output.removeAll({});
  for (const QString &str : output) {
    if (str.startsWith("info")) {
      QStringList line = str.split(' ');
      bool exit = false;
      for (int i = 0; i < line.size(); i++) {
        if (line[i] == "depth" && line[i + 1].toInt() == maxDepth)
          exit = true;
        if (line[i] == "cp") {
          int cp = line[i + 1].toInt();
          if (!evalWhite)
            cp = -cp;
          emit evalBarUpdate(std::clamp(cp, -1000, 1000), QString::number(static_cast<float>(cp) / 100.0f, 'f', 2));
        }
        else if (line[i] == "mate") {
          int mate = line[i + 1].toInt();
          if (!evalWhite)
            mate = -mate;
          emit evalBarUpdate(mate > 0 ? 1000 : -1000, "M" + QString::number(abs(mate)));
        }
        if (exit)
          break;
      }
      if (exit) {
        sendCommand("stop");
        break;
      }
    }
  }
}
