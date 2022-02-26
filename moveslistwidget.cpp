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
  pgnMoveString = "1. ";
}

void MovesListWidget::clearMoves() {
  clear();
  setRowCount(1);
  moveNumber = 0;
  turn = true;
  pgnMoveString = "1. ";
}

void MovesListWidget::updateSelected(int move) {
  QModelIndex index = model()->index(move / 2, move % 2);
  clearSelection();
  selectionModel()->select(index, QItemSelectionModel::Select);
}

void MovesListWidget::recordMove(const QString &move) {
  setItem(moveNumber, turn ? 0 : 1, new QTableWidgetItem(move));
  pgnMoveString += move + ' ';
  turn = !turn;
  if (turn) {
    insertRow(rowCount());
    moveNumber++;
    pgnMoveString += QString::number(moveNumber + 1) + ". ";
  }
}
