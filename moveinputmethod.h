#ifndef __MOVEINPUTMETHOD_H
#define __MOVEINPUTMETHOD_H

#include "uciengine.h"

class MoveInputMethod {
public:
  virtual void start(const QString &moves) {}
};

class UCIMoveInputMethod : public MoveInputMethod {
public:
  explicit UCIMoveInputMethod(const QString &path) : engine(path) {}
  void start(const QString &moves) override;

private:
  UCIEngine engine;
};

#endif
