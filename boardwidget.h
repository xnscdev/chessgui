#ifndef __BOARDWIDGET_H
#define __BOARDWIDGET_H

#include "aspectratiowidget.h"

class BoardWidget : public AspectRatioWidget {
private:
  class BoardWidgetBackend : public QWidget {
  public:
    explicit BoardWidgetBackend(QWidget *parent = nullptr) : QWidget(parent) {}
    void paintEvent(QPaintEvent *event) override;
  };

public:
  explicit BoardWidget(QWidget *parent = nullptr) : AspectRatioWidget(new BoardWidgetBackend, 1, 1, parent) {}
};

#endif
