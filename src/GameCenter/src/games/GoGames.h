#ifndef GOGAME_H
#define GOGAME_H

#include "AbstractGame/AbstractTableGame.h"
#include "GoRules.h"
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QVector>
#include <QSize>

class GoGames : public AbstractTableGame
{
    Q_OBJECT

public:
    explicit GoGames(QObject* parent = nullptr);

protected:
    void updateVisuals() override;
    void initializeGame() override;

private:
    void drawGrid();
    QSize gridSize_;
    int boardSize_; // Taille du plateau NxN
};

#endif // GOGAME_H
