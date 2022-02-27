#ifndef __BOARDWIDGET_H
#define __BOARDWIDGET_H

#include "aspectratiowidget.h"
#include "gamevariant.h"
#include "moveinputmethod.h"
#include "piece.h"
#include <QDate>
#include <QHash>
#include <QTableWidgetItem>

struct GameHistoryPosition {
  GamePosition pos;
  Move prevMove;
};

class BoardWidgetBackend : public QWidget {
  Q_OBJECT

public:
  bool orientation = false;
  QString pgnResult;
  QList<GameHistoryPosition> history;
  int historyMove;
  QString uciMoveString;

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
  QSettings settings;
  GameVariant &game;
  GamePosition position;
  bool canMove;
  bool turn;
  QPoint highlightedTile;
  QPoint moveFromTile;
  QPoint moveToTile;
  QPoint prevSelectedPiece;
  QPoint selectedPiece;
  QPoint ep;
  QList<QPoint> availableTiles;
  QHash<QPoint, Move> availableMovesMap;
  MoveInputMethod *whiteInputMethod;
  MoveInputMethod *blackInputMethod;

  QPoint selectedTile(QPoint pos);
  void drawPosition(QPainter &painter, GamePosition &position);
  void showAvailableMoves();
  void doMove(Move &move);
  bool tryMove(QPoint to);
  bool movablePieceAt(QPoint tile);
  Piece *promotePiece(Piece *oldPiece);
  bool findCheckmate();
  MoveInputMethod *createMoveInputMethod(const QString &key);

signals:
  void moveMade(const QString &move);

private slots:
  void receiveEngineMove(const QString &moveString);
};

class BoardWidget : public AspectRatioWidget {
  Q_OBJECT

public:
  explicit BoardWidget(QWidget *parent = nullptr);
  void reset() { backend->reset(); }
  static QString playerName(QSettings &settings, const QString &key, int defaultValue);
  [[nodiscard]] bool reversed() const { return backend->orientation; }
  [[nodiscard]] QString metadataPGN() const;
  [[nodiscard]] int historyMove() const;

private:
  BoardWidgetBackend *backend;

  QString fillPGNTag(const QSettings &settings, const QString &key) const;

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
