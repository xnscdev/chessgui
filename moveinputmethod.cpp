#include "moveinputmethod.h"
#include <QMessageBox>

void UCIMoveInputMethod::start(const QString &moves) {
  try {
    engine.sendCommand("position startpos moves" + moves);
    engine.sendCommand("go wtime 300000 btime 300000");
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
