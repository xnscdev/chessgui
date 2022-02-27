#ifndef __MOVEINPUTMETHOD_H
#define __MOVEINPUTMETHOD_H

#include "uciengine.h"

class MoveInputMethod : public QObject {
  Q_OBJECT

public:
  virtual void start(const QString &moves) {}
};

class UCIMoveInputMethod : public MoveInputMethod {
  Q_OBJECT

public:
  explicit UCIMoveInputMethod(const QString &path) : engine(path) {}
  void start(const QString &moves) override;

signals:
  void engineMoved(const QString &move);

private:
  UCIEngine engine;
};

#endif
