#ifndef __BOARDWIDGET_H
#define __BOARDWIDGET_H

#include "aspectratiowidget.h"
#include "gamevariant.h"
#include "piece.h"
#include <QHash>

class BoardWidgetBackend : public QWidget {
  Q_OBJECT

public:
  explicit BoardWidgetBackend(GameVariant &game, QWidget *parent = nullptr);
  void reset();

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
  QPoint prevSelectedPiece;
  QPoint selectedPiece;
  QPoint ep;
  QList<QPoint> availableTiles;
  QHash<QPoint, Move> availableMovesMap;

  QPoint selectedTile(QPoint pos);
  void showAvailableMoves();
  bool doMove(QPoint to);
  bool movablePieceAt(QPoint tile);
  void promotePiece(GamePiece &piece);
  void findCheckmate();

signals:
  void moveMade(const QString &move);
};

class BoardWidget : public AspectRatioWidget {
  Q_OBJECT

public:
  explicit BoardWidget(QWidget *parent = nullptr);
  void reset();

private:
  BoardWidgetBackend *backend;

signals:
  void moveMade(const QString &move);

private slots:
  void receiveMoveMade(const QString &move);
};

#endif
