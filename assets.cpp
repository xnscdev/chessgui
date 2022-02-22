#include "assets.h"
#include <QFileInfo>

QImage loadImage(const QString &path) {
  QImage image(path);
  if (!image.isNull())
    return image;
  return QImage(DATA_DIR "/" + path);
}

QSettings *loadSettings(const QString &path) {
  QFileInfo info(path);
  if (info.exists() && info.isFile())
    return new QSettings(path, QSettings::Format::IniFormat);
  else
    return new QSettings(DATA_DIR "/" + path, QSettings::Format::IniFormat);
}
