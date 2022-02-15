#ifndef __BOARDWIDGET_H
#define __BOARDWIDGET_H

#include "aspectratiowidget.h"
#include "gamevariant.h"
#include "piece.h"
#include <QMap>

class BoardWidget : public AspectRatioWidget {
private:
  class BoardWidgetBackend : public QWidget {
  public:
    explicit BoardWidgetBackend(GameVariant game, QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event) override;

  private:
    constexpr static const QColor lightColor{236, 207, 169};
    constexpr static const QColor darkColor{206, 144, 89};
    GameVariant game;
  };

public:
  explicit BoardWidget(QWidget *parent = nullptr)
      : AspectRatioWidget(new BoardWidgetBackend(loadedVariant), static_cast<float>(loadedVariant.size.width()),
                          static_cast<float>(loadedVariant.size.height()), parent) {}
};

#endif
