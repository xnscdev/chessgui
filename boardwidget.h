#ifndef __BOARDWIDGET_H
#define __BOARDWIDGET_H

#include "aspectratiowidget.h"
#include "gamevariant.h"
#include "moveinputmethod.h"
#include "piece.h"
#include "uciengine.h"
#include <QDate>
#include <QHash>
#include <QTableWidgetItem>
#include <QTimer>

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
  void newGame();
  void toFirstMove();
  void toPrevMove();
  void toNextMove();
  void toLastMove();
  void toMove(int move);
  void closeEngines();
  void whiteTimerTick();
  void blackTimerTick();

protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

private:
  constexpr static const QColor fromHighlightColor{255, 215, 0};
  constexpr static const QColor highlightColor{255, 232, 124};
  constexpr static const QColor lightColor{236, 207, 169};
  constexpr static const QColor darkColor{206, 144, 89};
  static const int timerFreq = 8;
  QSettings settings;
  GameVariant &game;
  GamePosition position;
  bool gameRunning = false;
  bool turn;
  QPoint highlightedTile;
  QPoint moveFromTile;
  QPoint moveToTile;
  QPoint prevSelectedPiece;
  QPoint selectedPiece;
  QPoint ep;
  QList<QPoint> availableTiles;
  QHash<QPoint, Move> availableMovesMap;
  QMap<QString, int> moveOccurrences;
  int lastIrreversibleMove;
  MoveInputMethod *whiteInputMethod = nullptr;
  MoveInputMethod *blackInputMethod = nullptr;
  UCIEngine *evalEngine = nullptr;
  QTimer *whiteTimer;
  QTimer *blackTimer;
  int whiteTime;
  int blackTime;
  int moveBonus;

  QPoint selectedTile(QPoint pos);
  void drawPosition(QPainter &painter, GamePosition &position);
  void showAvailableMoves();
  void doMove(Move &move);
  bool tryMove(QPoint to);
  bool movablePieceAt(QPoint tile);
  Piece *promotePiece(Piece *oldPiece);
  bool findGameEnd();
  static bool findDrawIM(QString ima, QString imb);
  void endGame(const QString &score, const QString &msg);
  [[nodiscard]] QString gameState() const;
  MoveInputMethod *createMoveInputMethod(const QString &key, bool white);
  UCIEngine *createEvalEngine();

signals:
  void moveMade(const QString &move);
  void whiteTimerTicked(int time);
  void blackTimerTicked(int time);
  void evalBarUpdate(int cp, const QString &label);

private slots:
  void receiveEngineMove(const QString &moveString);
  void receiveEvalBarUpdate(int cp, const QString &label) { emit evalBarUpdate(cp, label); }
};

class BoardWidget : public AspectRatioWidget {
  Q_OBJECT

public:
  explicit BoardWidget(QWidget *parent = nullptr);
  void newGame() { backend->newGame(); }
  static QString playerName(QSettings &settings, const QString &key, int defaultValue);
  [[nodiscard]] bool reversed() const { return backend->orientation; }
  [[nodiscard]] QString metadataPGN() const;
  [[nodiscard]] int historyMove() const;
  [[nodiscard]] QList<GameHistoryPosition> history() { return backend->history; }
  void closeEngines() { backend->closeEngines(); }
  [[nodiscard]] QString formattedTime(int ms) const;

private:
  BoardWidgetBackend *backend;

  [[nodiscard]] QString fillPGNTag(const QSettings &settings, const QString &key) const;

signals:
  void moveMade(const QString &move);
  void whiteTimerTicked(const QString &time);
  void blackTimerTicked(const QString &time);
  void evalBarUpdate(int cp, const QString &label);

private slots:
  void receiveMoveMade(const QString &move) { emit moveMade(move); };
  void receiveWhiteTimerTick(int time) { emit whiteTimerTicked(formattedTime(time)); };
  void receiveBlackTimerTick(int time) { emit blackTimerTicked(formattedTime(time)); }
  void receiveEvalBarUpdate(int cp, const QString &label) { emit evalBarUpdate(cp, label); }

public slots:
  void toFirstMove() { backend->toFirstMove(); }
  void toPrevMove() { backend->toPrevMove(); }
  void toNextMove() { backend->toNextMove(); }
  void toLastMove() { backend->toLastMove(); }
  void toMove(int move) { backend->toMove(move); }
};

#endif
