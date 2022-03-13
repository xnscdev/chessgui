#include "moveslistwidget.h"
#include <QHeaderView>

MovesListWidget::MovesListWidget(QWidget *parent) : QTableWidget(parent), moveNumber(0), turn(true) {
  setEditTriggers(QAbstractItemView::NoEditTriggers);
  setSelectionMode(QAbstractItemView::SingleSelection);
  horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  horizontalHeader()->hide();
  verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  setColumnCount(2);
  setRowCount(1);
  updateSelected(0);
  pgnMoveString.clear();
}

void MovesListWidget::clearMoves() {
  clear();
  setRowCount(1);
  clearSelection();
  selectionModel()->select(model()->index(0, 0), QItemSelectionModel::Select);
  moveNumber = 0;
  turn = true;
  pgnMoveString.clear();
}

void MovesListWidget::updateSelected(int move) {
  move--;
  QModelIndex index = model()->index(move / 2, move % 2);
  clearSelection();
  selectionModel()->select(index, QItemSelectionModel::Select);
}

void MovesListWidget::recordMove(const QString &move) {
  setItem(moveNumber, turn ? 0 : 1, new QTableWidgetItem(move));
  if (turn)
    pgnMoveString += QString::number(moveNumber + 1) + ". ";
  pgnMoveString += move + ' ';
  turn = !turn;
  if (turn) {
    insertRow(rowCount());
    moveNumber++;
  }
}
