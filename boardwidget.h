#ifndef __BOARDWIDGET_H
#define __BOARDWIDGET_H

#include "aspectratiowidget.h"
#include "gamevariant.h"
#include "piece.h"
#include <QHash>

class BoardWidget : public AspectRatioWidget {
private:
  class BoardWidgetBackend : public QWidget {
  public:
    explicit BoardWidgetBackend(GameVariant &game, QWidget *parent = nullptr);

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
    bool canMove = true;
    bool orientation = false;
    bool turn = true;
    QPoint highlightedTile{-1, 0};
    QPoint prevSelectedPiece{-1, 0};
    QPoint selectedPiece{-1, 0};
    QPoint ep{-1, 0};
    QList<QPoint> availableTiles;
    QHash<QPoint, Move> availableMovesMap;

    QPoint selectedTile(QPoint pos);
    void showAvailableMoves();
    bool doMove(QPoint to);
    bool movablePieceAt(QPoint tile);
    void promotePiece(GamePiece &piece);
    void findCheckmate();
  };

public:
  explicit BoardWidget(QWidget *parent = nullptr)
      : AspectRatioWidget(new BoardWidgetBackend(*loadedVariant), static_cast<float>(loadedVariant->size.width()),
                          static_cast<float>(loadedVariant->size.height()), parent) {}
};

#endif
