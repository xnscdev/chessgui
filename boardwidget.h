#ifndef __BOARDWIDGET_H
#define __BOARDWIDGET_H

#include <QBoxLayout>
#include <QGraphicsView>

class BoardWidget : public QWidget {
public:
  explicit BoardWidget(QWidget *parent = nullptr);
  void resizeEvent(QResizeEvent *event) override;

private:
  QGraphicsView *view;
  QBoxLayout *layout;
  float aspectWidth;
  float aspectHeight;
};

#endif
