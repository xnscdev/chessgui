#ifndef __MOVEINPUTMETHOD_H
#define __MOVEINPUTMETHOD_H

#include "uciengine.h"

class MoveInputMethod : public QObject {
  Q_OBJECT

public:
  virtual void start(const QString &moves, int whiteTime, int blackTime, int whiteMoveBonus, int blackMoveBonus) {}
  virtual void reset(bool useELO, int elo) {}
  [[nodiscard]] virtual bool manual() const { return true; }
};

class UCIMoveInputMethod : public MoveInputMethod {
  Q_OBJECT

public:
  explicit UCIMoveInputMethod(const QString &path, bool white) : engine(path, white) {}
  void start(const QString &moves, int whiteTime, int blackTime, int whiteMoveBonus, int blackMoveBonus) override;
  void reset(bool useELO, int elo) override;
  [[nodiscard]] bool manual() const override { return false; }

signals:
  void engineMoved(const QString &move);

private:
  UCIEngine engine;
};

#endif
