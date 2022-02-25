#ifndef __MOVESLISTWIDGET_H
#define __MOVESLISTWIDGET_H

#include <QTableWidget>

class MovesListWidget : public QTableWidget {
  Q_OBJECT

public:
  QString pgnMoveString;

  explicit MovesListWidget(QWidget *parent = nullptr);
  void clearMoves();

public slots:
  void recordMove(const QString &move);

private:
  int moveNumber;
  bool turn;
};

#endif
