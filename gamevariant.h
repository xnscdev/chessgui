#ifndef __GAMEVARIANT_H
#define __GAMEVARIANT_H

#include <QSize>

struct GameVariant {
  QSize size;
};

extern GameVariant *loadedVariant;

#endif
