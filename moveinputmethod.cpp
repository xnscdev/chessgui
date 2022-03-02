#include "moveinputmethod.h"
#include <QMessageBox>

void UCIMoveInputMethod::start(const QString &moves, int whiteTime, int blackTime, int moveBonus) {
  if (!whiteTime)
    whiteTime = 600000;
  if (!blackTime)
    blackTime = 600000;
  engine.sendCommand("position startpos moves" + moves);
  engine.sendCommand(
      QStringLiteral("go wtime %1 btime %2 winc %3 binc %3").arg(whiteTime).arg(blackTime).arg(moveBonus));
  emit engineMoved(engine.waitMove());
}

void UCIMoveInputMethod::reset(bool useELO, int elo) {
  if (useELO) {
    engine.sendCommand("setoption name UCI_LimitStrength value true");
    engine.sendCommand("setoption name UCI_Elo value " + QString::number(elo));
  }
  engine.sendCommand("ucinewgame");
  engine.sendCommand("isready");
  engine.waitResponse("readyok");
}
