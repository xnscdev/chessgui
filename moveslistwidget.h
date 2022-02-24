#ifndef __MOVESLISTWIDGET_H
#define __MOVESLISTWIDGET_H

#include <QTableWidget>

class MovesListWidget : public QTableWidget {
  Q_OBJECT

public:
  explicit MovesListWidget(QWidget *parent = nullptr);

public slots:
  void recordMove(const QString &move);

private:
  int moveNumber;
  bool turn;
};

#endif
