#ifndef __ASSETS_H
#define __ASSETS_H

#include <QImage>
#include <QSettings>

QImage loadImage(const QString &path);
QSettings *loadSettings(const QString &path);

#endif
