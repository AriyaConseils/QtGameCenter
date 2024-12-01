#include "playerroom.h"
#include "ui_playerroom.h"
#include "GameCatalogModel.h"
#include "fireworksanimation.h"
#include "bots/RandomBot.h"
#include <QMessageBox>
#include <QDateTime>
#include <QJsonArray>
#include <QResizeEvent>


PlayerRoom::PlayerRoom(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayerRoom)
{
    ui->setupUi(this);
    ui->widgetGameSeat1->changeLayoutOrder(PlayerStatistics::AlternateOrder);

    ui->comboBoxGameCatalog->setModel(new GameCatalogModel(this));

    ui->graphicsView->setAttribute(Qt::WA_TranslucentBackground, true);
    ui->graphicsView->setStyleSheet("background: transparent; border: none;");
    ui->graphicsView->setFrameStyle(QFrame::NoFrame);

    connect(ui->comboBoxGameCatalog, &QComboBox::currentIndexChanged, this, &PlayerRoom::onGameChanged);

    playerSeatMap_.insert(PlayerEnums::Player1, ui->widgetGameSeat1);
    playerSeatMap_.insert(PlayerEnums::Player2, ui->widgetGameSeat2);
}

PlayerRoom::~PlayerRoom()
{
    delete ui;

    currentGame_.reset();
}





void PlayerRoom::setLocalPlayer(Player *player)
{
    // Sets the local player and connects their properties
    // to their seat in the UI using CONNECT_PROP for real-time updates.

    localPlayer_ = player;

    // Initialize property bindings
    CONNECT_PROP(Player, player, online,      playerSeatMap_.value(PlayerEnums::Player1), online);
    CONNECT_PROP(Player, player, firstName,   playerSeatMap_.value(PlayerEnums::Player1), firstName);
    CONNECT_PROP(Player, player, lastName,    playerSeatMap_.value(PlayerEnums::Player1), lastName);
    CONNECT_PROP(Player, player, gamesWon,    playerSeatMap_.value(PlayerEnums::Player1), gamesWon);
    CONNECT_PROP(Player, player, gamesPlayed, playerSeatMap_.value(PlayerEnums::Player1), gamesPlayed);
    CONNECT_PROP(Player, player, image,       playerSeatMap_.value(PlayerEnums::Player1), image);
}


bool PlayerRoom::addPlayer(Player *player, bool managed)
{
    // Adds a new player to the room and connects their properties
    // to their seat in the UI. Uses CONNECT_PROP to bind properties
    // dynamically for real-time updates in the interface.

    if (currentGame_ &&
        (currentGame_->rulesOfTheGame()->numberMaxPlayers() - 1 /*Local player*/) > playerManagementMap_.keys().size()) {

        // Add the new player to the management map
        playerManagementMap_.insert(player, managed);
        int totalPlayers = playerManagementMap_.keys().size() + 1;

        // Update room information
        QJsonArray playersArray = roomInfo_["players"].toArray();
        playersArray.append(player->toJson());
        roomInfo_["playerCount"] = totalPlayers;
        roomInfo_["players"] = playersArray;

        // Connect the player's properties to their seat in the UI
        CONNECT_PROP(Player, player, online,      playerSeatMap_.value(PlayerEnums::Player2), online);
        CONNECT_PROP(Player, player, firstName,   playerSeatMap_.value(PlayerEnums::Player2), firstName);
        CONNECT_PROP(Player, player, lastName,    playerSeatMap_.value(PlayerEnums::Player2), lastName);
        CONNECT_PROP(Player, player, gamesWon,    playerSeatMap_.value(PlayerEnums::Player2), gamesWon);
        CONNECT_PROP(Player, player, gamesPlayed, playerSeatMap_.value(PlayerEnums::Player2), gamesPlayed);
        CONNECT_PROP(Player, player, image,       playerSeatMap_.value(PlayerEnums::Player2), image);

        // Show the player's seat in the UI
        playerSeatMap_.value(PlayerEnums::Player2)->show();

        // Notify that the room information has been updated
        emit roomInfoChanged(roomInfo_);

        return true;
    }

    // Return false if adding the player is not possible
    return false;
}



void PlayerRoom::initRoom()
{
    // Initializes the room, resets the game state, and notifies the server
    // that the room is ready and valid. Prepares the room data and updates the view.

    // Reset the game state and clear the player management map
    resetGame();
    playerManagementMap_.clear();

    // Build room information as a JSON object
    roomInfo_ = QJsonObject();
    roomInfo_["uuid"] = QUuid::createUuid().toString(QUuid::WithoutBraces);
    roomInfo_["playerCount"] = playerManagementMap_.keys().size() + 1; // Includes the local player
    roomInfo_["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    roomInfo_["active"] = true;
    roomInfo_["playing"] = false;

    // Add the local player to the room's players list
    QJsonArray playersArray;
    playersArray.append(localPlayer_->toJson());
    roomInfo_["players"] = playersArray;

    // Update the game view with the selected game
    onGameChanged(ui->comboBoxGameCatalog->currentIndex());

    // Hide the second player's seat until another player joins
    playerSeatMap_.value(PlayerEnums::Player2)->hide();
}



void PlayerRoom::onGameChanged(int index)
{
    // Validate the selected index and the local player
    if (index < 0 || !localPlayer_) return;

    // Retrieve the game factory associated with the selected item
    QVariant data = ui->comboBoxGameCatalog->itemData(index);
    if (!data.isValid()) return;

    // Reset the current game
    resetGame();

    // Create the new game using the factory
    auto factory = data.value<GameFactory>();
    currentGame_ = factory();
    if (!currentGame_) return;

    // Update UI with the game rules
    ui->textEditRules->setHtml(currentGame_->rulesOfTheGame()->gameRules());

    // Connect game signals
    connectGameSignals();

    // Display the new game scene
    ui->graphicsView->setScene(currentGame_.get());
    ui->graphicsView->fitInView(currentGame_->sceneRect(), Qt::KeepAspectRatio);

    // Update room information
    roomInfo_["game"] = ui->comboBoxGameCatalog->currentText();
    roomInfo_["playing"] = false;
    emit roomInfoChanged(roomInfo_);
}

void PlayerRoom::connectGameSignals()
{
    // Connect the signal for player movement
    connect(currentGame_.get(), &AbstractTableGame::playerMoved, [](Player player, const QPoint &position) {
        qDebug() << "Player" << player << "moved to position" << position;
    });

    // Connect the signal for game over
    connect(currentGame_.get(), &AbstractTableGame::gameOver, this, [this](PlayerEnums::PlayerPosition winner) {
        handleGameOver(winner);
    });
}


void PlayerRoom::handleGameOver(PlayerEnums::PlayerPosition winner)
{
    // Create and configure fireworks animation
    FireworksAnimation* fireworks = new FireworksAnimation(this);
    fireworks->setGeometry(ui->graphicsView->geometry());

    // Update player statistics based on game result
    localPlayer_->setGamesPlayed(localPlayer_->gamesPlayed() + 1);
    if (winner == PlayerEnums::Unknown) {
        fireworks->startAnimation(3000, "DRAW", false);
    } else if (localPlayer_->playerPosition() == winner) {
        fireworks->startAnimation(4000, "YOU WIN", true);
        localPlayer_->setGamesWon(localPlayer_->gamesWon() + 1);
    } else {
        fireworks->startAnimation(3000, "YOU LOOSE", false);
    }

    // Handle end of animation
    connect(fireworks, &FireworksAnimation::animationFinished, this, [this, fireworks]() {
        fireworks->deleteLater();
        ui->stackedWidgetRoom->setCurrentIndex(0);
        roomInfo_["playing"] = false;
        emit roomInfoChanged(roomInfo_);
    });
}


void PlayerRoom::on_pushButtonPlay_clicked()
{
    // Retrieve the current game scene from the graphics view
    auto* game = dynamic_cast<AbstractTableGame*>(ui->graphicsView->scene());
    if (!game) {
        qDebug() << "The current scene is not an AbstractTableGame.";
        return;
    }

    // Check if the required number of players is met
    int playersNumber = playerManagementMap_.keys().size();
    int requiredPlayers = game->rulesOfTheGame()->numberMaxPlayers() - 1; // Local player excluded
    if (playersNumber != requiredPlayers) {
        QMessageBox::information(
            this,
            "Missing Players",
            QString("The game needs %1 more player(s) to start.")
                .arg(requiredPlayers - playersNumber),
            QMessageBox::Ok
            );
        return;
    }

    // Start the game if it is not already in progress
    if (!roomInfo_["playing"].toBool()) {
        setupPlayersForGame(game);
        game->startGame();
        roomInfo_["playing"] = true;
        emit roomInfoChanged(roomInfo_);
    }

    // Switch to the game view and adjust the scene display
    ui->stackedWidgetRoom->setCurrentIndex(1);
    ui->graphicsView->fitInView(game->sceneRect(), Qt::KeepAspectRatio);
}

void PlayerRoom::setupPlayersForGame(AbstractTableGame* game)
{
    // Add local player to the game
    currentGame_->managePlayer(localPlayer_);

    // Add other players or bots to the game
    for (Player* player : playerManagementMap_.keys()) {
        if (playerManagementMap_.value(player)) {
            currentGame_->managePlayer(player);
        } else {
            currentGame_->addPlayer(player);
        }

        // Assign the game instance to bots
        if (auto* bot = dynamic_cast<AbstractBot*>(player)) {
            bot->setGame(currentGame_.get());
        }
    }
}




void PlayerRoom::resizeEvent(QResizeEvent *event)
{
    // Dynamically manages the resizing of the widget. Adjusts
    // the player seats' layout based on the window height and
    // ensures the current game scene fits correctly in the view.

    QSize newSize = event->size();

    for (auto seatKey : playerSeatMap_.keys()) {
        if (auto seatWidget = playerSeatMap_.value(seatKey)) {
            seatWidget->setFullSize(newSize.height() > 600);
        }
    }

    if (currentGame_) {
        ui->graphicsView->fitInView(currentGame_->sceneRect(), Qt::KeepAspectRatio);
    }

    QWidget::resizeEvent(event);
}



bool PlayerRoom::getBack()
{
    // Function invoked by the parent to navigate back. If the
    // current index in the stacked widget is greater than 0,
    // it moves back to the previous view. Otherwise, it clears
    // the room and returns false to indicate no further navigation.
    if (ui->stackedWidgetRoom->currentIndex() > 0) {
        ui->stackedWidgetRoom->setCurrentIndex(ui->stackedWidgetRoom->currentIndex() - 1);
        return true;
    }
    clearRoom();
    return false;
}


void PlayerRoom::clearRoom()
{
    // Cleans up the room and informs the server that the room
    // is no longer active. This includes resetting the game
    // state, clearing player data, and updating the room info.

    resetGame();

    if (!roomInfo_.isEmpty()) {
        roomInfo_["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
        roomInfo_["active"] = false;
        roomInfo_["players"] = QJsonArray();
        roomInfo_["playing"] = false;

        // Emit signal to notify that the room state has changed.
        emit roomInfoChanged(roomInfo_);

        // Clear the room information completely.
        roomInfo_ = QJsonObject();
    }
}


void PlayerRoom::resetGame()
{
    if (currentGame_) {
        // Detaches the current game scene from the graphics view.
        ui->graphicsView->setScene(nullptr);

        // Resets the unique_ptr, which automatically deletes the current game.
        currentGame_.reset();
    }
}


void PlayerRoom::on_pushButtonCreatBot_clicked()
{
    // Creates a new random bot named "John Doe" with a preset avatar
    // and adds it to the room. If the maximum number of players is
    // reached, a message box informs the user that no additional bots
    // can be added.
    RandomBot* John = new RandomBot(this);
    John->setFirstName("John");
    John->setLastName("Doe");
    John->setOnline(true);
    John->setImage(":/Avatars/assets/masque.png");

    if (!addPlayer(John)) {
        QMessageBox::information(this,
                                 "Maximum Players Reached",
                                 "It is not possible to add another bot. The maximum number of players has already been reached.");
        return;
    }
}


void PlayerRoom::on_pushButtonAddPlayer_clicked()
{
    // Creates a new player named "Haavard Nord" with a preset avatar
    // and adds them to the room. If the maximum number of players
    // is reached, a message box notifies the user that no more players
    // can be added.
    Player *Haavard = new Player("Haavard", "Nord");
    Haavard->setImage(":/Avatars/assets/pere.png");

    Haavard->setOnline("true");
    if (!addPlayer(Haavard, true)) {
        QMessageBox::information(this,
                                 "Maximum Players Reached",
                                 "It is not possible to add another bot. The maximum number of players has already been reached.");
        return;
    }
}

