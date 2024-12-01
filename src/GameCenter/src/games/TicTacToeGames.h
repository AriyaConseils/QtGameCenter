#ifndef TICTACTOGAME_H
#define TICTACTOGAME_H

#include "AbstractGame/AbstractTableGame.h"
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QVector>
#include <QSize>
#include "TicTacToeRules.h"

class TicTacToeGames : public AbstractTableGame
{
    Q_OBJECT

public:
    explicit TicTacToeGames(QObject* parent = nullptr);

protected:
    void updateVisuals() override;
    void initializeGame() override;

private:
    void drawGrid();
    QString playerToString(PlayerEnums::PlayerPosition player) const;
    QSize gridSize_;

};

#endif // TICTACTOGAME_H
