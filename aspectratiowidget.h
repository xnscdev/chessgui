#ifndef __ASPECTRATIOWIDGET_H
#define __ASPECTRATIOWIDGET_H

#include <QBoxLayout>
#include <QGraphicsView>

class AspectRatioWidget : public QWidget {
public:
  AspectRatioWidget(QWidget *widget, float aspectWidth, float aspectHeight, QWidget *parent = nullptr);
  void resizeEvent(QResizeEvent *event) override;
  QWidget *&widget() { return mWidget; }

private:
  QWidget *mWidget;
  QBoxLayout *layout;
  float aspectWidth;
  float aspectHeight;
};

#endif
