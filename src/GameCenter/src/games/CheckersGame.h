#ifndef CHECKERSGAME_H
#define CHECKERSGAME_H

#include "AbstractGame/AbstractTableGame.h"
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QVector>
#include <QSize>
#include "CheckersRules.h"

class CheckersGame : public AbstractTableGame
{
    Q_OBJECT

public:
    explicit CheckersGame(QObject* parent = nullptr);

protected:
    void updateVisuals() override;
    void initializeGame() override;

private:
    void drawGrid();
    void initializePieces();
    void cleanUpGame();
    QString pieceToString(AbstractStrategyRulesGame::AtomicCellState cellState) const;
    QSize gridSize_;
};

#endif // CHECKERSGAME_H
