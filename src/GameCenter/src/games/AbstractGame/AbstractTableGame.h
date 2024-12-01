#ifndef ABSTRACTTABLEGAME_H
#define ABSTRACTTABLEGAME_H

#include <QGraphicsScene>
#include <QVector>
#include <algorithm>
#include "Player.h"
#include "AbstractStrategyRulesGame.h"
#include "framework/types/QCircularList.h"
#include "framework/types/QMatrix.h"
#include "framework/helpers.h"

/**
 * @class AbstractTableGame
 * @brief Abstract class for managing the logic of a board game.
 *
 * This class provides the foundation for handling rules, players, and moves
 * in a board game, along with graphical updates and interaction.
 */
class AbstractTableGame : public QGraphicsScene {
    Q_OBJECT
    DECLARE_PROPERTY(bool, gameOpen, setGameOpen, false)

public:
    /**
     * @brief Constructor.
     * @param parent Pointer to the parent QObject.
     */
    explicit AbstractTableGame(QObject* parent = nullptr);

    /**
     * @brief Virtual destructor.
     */
    virtual ~AbstractTableGame() {}

    /**
     * @brief Installs the game rules.
     * @param rules Pointer to the game rules to install.
     */
    virtual void installRules(AbstractStrategyRulesGame *rules);

    /**
     * @brief Resets the game to its initial state.
     */
    virtual void startGame();

    /**
     * @brief Updates the visuals of the board.
     * @param player The player whose move to reflect.
     * @param row The row of the move.
     * @param col The column of the move.
     */
    virtual void updateVisuals() = 0;

    /**
     * @brief Initializes the game setup (e.g., pieces, players).
     */
    virtual void initializeGame() = 0;

    /**
     * @brief Manages a player in the game.
     * @param player The player to manage.
     */
    virtual bool managePlayer(Player *player);

    /**
     * @brief Adds a new player to the game.
     * @param player The player to add.
     * @return The position assigned to the player.
     */
    virtual bool addPlayer(Player *player);

    /**
     * @brief Executes a move in the game.
     * @param player The player making the move.
     * @param originalPosition The initial position of the piece.
     * @param nextPosition The destination position of the piece.
     * @return True if the move is valid and executed, false otherwise.
     */
    bool playMove(Player *player, QPoint originalPosition, QPoint nextPosition);

    /**
     * @brief Gets the current player.
     * @return The current player.
     */
    Player *currentPlayer() const { return players_.current(); }

    /**
     * @brief Gets the current game state.
     * @return A const reference to the game state matrix.
     */
    const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& getGameState() const { return gameState_; }

    /**
     * @brief Gets the rules of the game.
     * @return A pointer to the game rules.
     */
    AbstractStrategyRulesGame *rulesOfTheGame() const;

protected:
    /**
     * @brief Handles mouse press events for interaction.
     * @param event Pointer to the mouse event.
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * @brief Handles mouse move events for drag-and-drop interaction.
     * @param event Pointer to the mouse event.
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * @brief Handles mouse release events to finalize interactions.
     * @param event Pointer to the mouse event.
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    /**
     * @brief Emitted when a player has made a move.
     * @param player The player who made the move.
     * @param playedPos The position of the move.
     */
    void playerMoved(Player *player, const QPoint &playedPos);

    /**
     * @brief Emitted when a move is invalid.
     * @param player The player attempting the invalid move.
     */
    void invalidMove(Player *player);

    /**
     * @brief Emitted when the game ends.
     * @param winner The position of the winning player.
     */
    void gameOver(PlayerEnums::PlayerPosition winner);

    /**
     * @brief Emitted when the number of players is insufficient.
     * @param nbPlayers The number of players currently in the game.
     */
    void missingPlayers(int nbPlayers);

    /**
     * @brief Emitted when the maximum number of players is reached.
     */
    void maxPlayerAllowed();

    /**
     * @brief Emitted when the game open status changes.
     */
    void gameOpenChanged();

protected:
    AbstractStrategyRulesGame *rulesOfTheGame_; ///< Pointer to the game rules.

    QMatrix<AbstractStrategyRulesGame::AtomicCellState> gameState_; ///< Current game state matrix.
    QMatrix<QGraphicsRectItem*> cells_; ///< Visual representation of the game grid.

    QCircularList<Player *> players_; ///< List of players in the game.
    QList<Player *> managedPlayers; ///< Players managed by the game.

private:
    /**
     * @brief Maps a position from the scene to the game grid.
     * @param position The position in the scene.
     * @return The corresponding grid position.
     */
    QPoint mapPositionToGame(QPointF position);

    /**
     * @brief Checks if an item is a game piece.
     * @param item The graphics item to check.
     * @return True if the item is a game piece, false otherwise.
     */
    bool isGamePiece(QGraphicsItem* item) const;

private:
    QPointF originalPos_; ///< Original position of the dragged item.
    QGraphicsItem *draggedItem_; ///< Currently dragged game piece.
};

#endif // ABSTRACTTABLEGAME_H
