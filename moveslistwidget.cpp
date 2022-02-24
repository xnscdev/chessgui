#include "moveslistwidget.h"
#include <QHeaderView>

MovesListWidget::MovesListWidget(QWidget *parent) : QTableWidget(parent), moveNumber(0), turn(true) {
  setEditTriggers(QAbstractItemView::NoEditTriggers);
  horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  horizontalHeader()->hide();
  verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  setColumnCount(2);
  setRowCount(1);
}

void MovesListWidget::clearMoves() {
  clear();
  setRowCount(1);
  moveNumber = 0;
  turn = true;
}

void MovesListWidget::recordMove(const QString &move) {
  setItem(moveNumber, turn ? 0 : 1, new QTableWidgetItem(move));
  turn = !turn;
  if (turn) {
    insertRow(rowCount());
    moveNumber++;
  }
}
