#include "AbstractBot.h"

/**
 * @brief Constructs an AbstractBot.
 * @param parent The parent QObject (default: nullptr).
 */
AbstractBot::AbstractBot(QObject* parent)
    : Player(parent)
    , gameScene(nullptr) {

}

/**
 * @brief Destructor for AbstractBot.
 */
AbstractBot::~AbstractBot() {}

/**
 * @brief Connects the bot to a game scene.
 * @param scene A pointer to the AbstractTableGame scene.
 */
void AbstractBot::setGame(AbstractTableGame* scene) {
    gameScene = scene;

    if (gameScene) {
        // Lambda function to play if it's the bot's turn
        auto playIfCurrentPlayer = [this]() {
            if (gameScene->gameOpen() && gameScene->currentPlayer() == this) {
                play(gameScene->getGameState());
            }
        };

        // Connect signals to trigger bot's move logic
        connect(gameScene, &AbstractTableGame::gameOpenChanged, this, playIfCurrentPlayer, Qt::QueuedConnection);
        connect(gameScene, &AbstractTableGame::playerMoved, this, playIfCurrentPlayer, Qt::QueuedConnection);
    }
}
