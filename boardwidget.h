#ifndef __BOARDWIDGET_H
#define __BOARDWIDGET_H

#include "aspectratiowidget.h"
#include "gamevariant.h"
#include "piece.h"
#include <QDate>
#include <QHash>
#include <QTableWidgetItem>

struct GameHistoryPosition {
  GamePosition pos;
  Move prevMove;
};

struct GameMetadata {
  QString event = "?";
  QString site = "?";
  QDate date = QDate::currentDate();
  QString round = "?";
  QString whitePlayer = "?";
  QString blackPlayer = "?";
  QString result = "*";
};

class BoardWidgetBackend : public QWidget {
  Q_OBJECT

public:
  GameMetadata metadata;
  QList<GameHistoryPosition> history;
  int historyMove;

  explicit BoardWidgetBackend(GameVariant &game, QWidget *parent = nullptr);
  void reset();
  void toFirstMove();
  void toPrevMove();
  void toNextMove();
  void toLastMove();
  void toMove(int move);

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

private:
  constexpr static const QColor highlightColor{255, 232, 124};
  constexpr static const QColor lightColor{236, 207, 169};
  constexpr static const QColor darkColor{206, 144, 89};
  GameVariant &game;
  GamePosition position;
  bool orientation = false;
  bool canMove;
  bool turn;
  QPoint highlightedTile;
  QPoint moveFromTile;
  QPoint prevSelectedPiece;
  QPoint selectedPiece;
  QPoint ep;
  QList<QPoint> availableTiles;
  QHash<QPoint, Move> availableMovesMap;

  QPoint selectedTile(QPoint pos);
  void drawPosition(QPainter &painter, GamePosition &position);
  void showAvailableMoves();
  bool doMove(QPoint to);
  bool movablePieceAt(QPoint tile);
  Piece *promotePiece(Piece *oldPiece);
  void findCheckmate();

signals:
  void moveMade(const QString &move);
};

class BoardWidget : public AspectRatioWidget {
  Q_OBJECT

public:
  explicit BoardWidget(QWidget *parent = nullptr);
  void reset() { backend->reset(); }
  QString metadataPGN() const;
  int historyMove();

private:
  BoardWidgetBackend *backend;

signals:
  void moveMade(const QString &move);

private slots:
  void receiveMoveMade(const QString &move);

public slots:
  void toFirstMove() { backend->toFirstMove(); }
  void toPrevMove() { backend->toPrevMove(); }
  void toNextMove() { backend->toNextMove(); }
  void toLastMove() { backend->toLastMove(); }
  void toMove(int move) { backend->toMove(move); }
};

#endif
