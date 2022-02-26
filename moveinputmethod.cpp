#include "moveinputmethod.h"

void UCIMoveInputMethod::start(const QString &moves) {
  engine.sendCommand("position startpos moves" + moves);
  engine.sendCommand("go wtime 300000 btime 300000");
}
