#ifndef ABSTRACTBOT_H
#define ABSTRACTBOT_H

#include <QVector>
#include <QObject>
#include "games/AbstractGame/AbstractTableGame.h"

/**
 * @class AbstractBot
 * @brief Represents a base class for creating bots in a table game.
 *
 * AbstractBot provides a framework for implementing game-playing bots.
 * Subclasses must implement the `play` method to define the bot's logic.
 */
class AbstractBot : public Player {
    Q_OBJECT

public:
    /**
     * @brief Constructs an AbstractBot.
     * @param parent The parent QObject (default: nullptr).
     */
    explicit AbstractBot(QObject* parent = nullptr);

    /**
     * @brief Destructor for AbstractBot.
     */
    virtual ~AbstractBot();

    /**
     * @brief Pure virtual function to define the bot's move logic.
     * @param gameState The current state of the game board.
     * @return The position on the board where the bot decides to play.
     */
    virtual QPoint play(const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& gameState) = 0;

    /**
     * @brief Connects the bot to a game scene.
     * @param scene A pointer to the AbstractTableGame scene.
     */
    virtual void setGame(AbstractTableGame* scene);

protected:
    AbstractTableGame* gameScene; ///< Pointer to the game scene the bot is playing in.

signals:
    /**
     * @brief Emitted when the bot makes a move.
     * @param position The position played by the bot.
     */
    void botPlayed(int position);
};

#endif // ABSTRACTBOT_H
