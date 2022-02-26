#include "uciengine.h"

UCIEngine::UCIEngine(const QString &path) {
  process.start(path);
  process.setReadChannel(QProcess::StandardOutput);
  sendCommand("uci");
}

QStringList UCIEngine::sendCommand(const QString &cmd) {
  QStringList list;
  process.write((cmd + '\n').toUtf8());
  while (process.canReadLine()) {
    QString line = QString::fromLocal8Bit(process.readLine());
    list.append(line);
  }
  return list;
}
