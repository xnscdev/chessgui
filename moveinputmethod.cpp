#include "moveinputmethod.h"
#include <QMessageBox>

void UCIMoveInputMethod::start(const QString &moves, int whiteTime, int blackTime) {
  if (!whiteTime)
    whiteTime = 600000;
  if (!blackTime)
    blackTime = 600000;
  try {
    engine.sendCommand("position startpos moves" + moves);
    engine.sendCommand(QStringLiteral("go wtime %1 btime %2").arg(whiteTime).arg( blackTime));
    emit engineMoved(engine.waitMove());
  }
  catch (UCIException &) {
    uciError();
  }
}

void UCIMoveInputMethod::reset(bool useELO, int elo) {
  try {
    if (useELO) {
      engine.sendCommand("setoption name UCI_LimitStrength value true");
      engine.sendCommand("setoption name UCI_Elo value " + QString::number(elo));
    }
    engine.sendCommand("ucinewgame");
    engine.sendCommand("isready");
    engine.waitResponse("readyok");
  }
  catch (UCIException &) {
    uciError();
    exit(0);
  }
}

void UCIMoveInputMethod::uciError() const {
  QMessageBox box;
  box.setIcon(QMessageBox::Critical);
  box.setText("UCI Error");
  box.setInformativeText("An error occurred while communicating with the engine.");
  box.exec();
}
