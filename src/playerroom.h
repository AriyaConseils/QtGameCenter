#ifndef PLAYERROOM_H
#define PLAYERROOM_H

#include <QWidget>
#include <QMap>
#include <QJsonObject>
#include "games/AbstractGame/AbstractTableGame.h"
#include "playerstatistics.h"

namespace Ui {
class PlayerRoom;
}

/**
 * @class PlayerRoom
 * @brief Represents the game room where players can join and start a match.
 */
class PlayerRoom : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerRoom(QWidget *parent = nullptr);
    ~PlayerRoom();

    /**
     * @brief Sets the local player for the room.
     * @param player Pointer to the local player instance.
     */
    void setLocalPlayer(Player *player);

    /**
     * @brief Handles the back navigation logic.
     * @return True if the navigation succeeded, false otherwise.
     */
    Q_INVOKABLE bool getBack();

    /**
     * @brief Adds a player to the room.
     * @param player Pointer to the player instance.
     * @param managed True if the player is managed by the server.
     * @return True if the player was added successfully, false otherwise.
     */
    bool addPlayer(Player *player, bool managed = false);

    /**
     * @brief Initializes the room with default settings and data.
     */
    void initRoom();

    /**
     * @brief Clears the room and notifies that it is closed.
     */
    void clearRoom();

protected:
    /**
     * @brief Handles window resize events.
     * @param event The resize event.
     */
    void resizeEvent(QResizeEvent *event) override;

signals:
    /**
     * @brief Emitted when the room information changes.
     * @param roomInfo The updated room information as a JSON object.
     */
    void roomInfoChanged(const QJsonObject &roomInfo);

private slots:
    /**
     * @brief Updates the game when the selected game changes.
     * @param index The index of the selected game.
     */
    void onGameChanged(int index);

    /**
     * @brief Starts the game if all conditions are met.
     */
    void on_pushButtonPlay_clicked();

    /**
     * @brief Resets the current game.
     */
    void resetGame();

    /**
     * @brief Creates a bot and adds it to the room.
     */
    void on_pushButtonCreatBot_clicked();

    /**
     * @brief Adds a new player to the room manually.
     */
    void on_pushButtonAddPlayer_clicked();

    /**
     * @brief Handles the end-of-game logic based on the winner's position.
     *
     * This function is triggered when the game is over. It updates the player's statistics,
     * displays a fireworks animation indicating the result (win, lose, or draw), and resets
     * the room's state after the animation finishes.
     *
     * @param winner The position of the winning player in the game. If unknown, the game is considered a draw.
     */
    void handleGameOver(PlayerEnums::PlayerPosition winner);

private:
    Ui::PlayerRoom *ui; ///< Pointer to the UI instance.
    QJsonObject roomInfo_; ///< JSON object holding the current room's information.
    std::unique_ptr<AbstractTableGame> currentGame_; ///< Pointer to the current game scene.
    Player *localPlayer_ = nullptr; ///< Pointer to the local player instance.
    QMap<Player *, bool> playerManagementMap_; ///< Map of players in the room and their management status (managed/unmanaged).
    QMap<PlayerEnums::PlayerPosition, PlayerStatistics *> playerSeatMap_; ///< Map linking player positions to their corresponding seat widgets.

    /**
     * @brief Returns a stylesheet for a status widget based on a given color.
     * @param color The color to apply.
     * @return A QString containing the stylesheet.
     */
    QString getStatusStyleSheet(const QString &color) const;

    /**
     * @brief Prepares players and bots for the game by managing or adding them to the scene.
     * @param game Pointer to the current game scene.
     */
    void setupPlayersForGame(AbstractTableGame* game);

    /**
     * @brief Connects signals from the current game to the appropriate handlers.
     *
     * This function establishes connections for key game events such as player movement
     * and the end of the game. It ensures that the necessary responses are triggered
     * when these events occur during the game.
     */
    void connectGameSignals();



};

#endif // PLAYERROOM_H
