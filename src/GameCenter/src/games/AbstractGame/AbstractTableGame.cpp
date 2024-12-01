#include "AbstractTableGame.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QCursor>

AbstractTableGame::AbstractTableGame(QObject* parent)
    : QGraphicsScene(parent)
    , rulesOfTheGame_(nullptr)
    , draggedItem_(nullptr)
{}

// --------------------------------------------
// Rules Installation and Game Reset
// --------------------------------------------

void AbstractTableGame::installRules(AbstractStrategyRulesGame* rules) {
    rulesOfTheGame_ = rules;
}

void AbstractTableGame::startGame() {
    Q_ASSERT(rulesOfTheGame_);

    // Check for the minimum number of players
    if (rulesOfTheGame_->numberMinPlayers() > players_.size()) {
        emit missingPlayers(rulesOfTheGame_->numberMinPlayers() - players_.size());
        return;
    }

    // Initialize game state
    initializeGame();
    gameState_ = rulesOfTheGame_->initStatePlayers(cells_.rows(), cells_.cols());
    setGameOpen(true);
}

// --------------------------------------------
// Getter
// --------------------------------------------

AbstractStrategyRulesGame *AbstractTableGame::rulesOfTheGame() const {
    return rulesOfTheGame_;
}


// --------------------------------------------
// Move Logic
// --------------------------------------------

bool AbstractTableGame::playMove(Player *player, QPoint originalPosition, QPoint nextPosition) {
    Q_ASSERT(rulesOfTheGame_);

    // Validate positions
    if (originalPosition.ry() < 0 || originalPosition.ry() >= gameState_.rows() ||
        originalPosition.rx() < 0 || originalPosition.rx() >= gameState_.cols() ||
        nextPosition.ry() < 0 || nextPosition.ry() >= gameState_.rows() ||
        nextPosition.rx() < 0 || nextPosition.rx() >= gameState_.cols()) {
        emit invalidMove(player);
        return false;
    }

    if (player != players_.current()) {
        emit invalidMove(player);
        return false;
    }

    // Prepare the next game state
    QMatrix<AbstractStrategyRulesGame::AtomicCellState> nextGameState_ = gameState_;
    AbstractStrategyRulesGame::AtomicCellState oldCell = nextGameState_.at(originalPosition.ry(), originalPosition.rx());
    nextGameState_.at(originalPosition.ry(), originalPosition.rx()) = { PlayerEnums::Unknown, 0, nullptr };
    nextGameState_.at(nextPosition.ry(), nextPosition.rx()) = { player->playerPosition(), oldCell.typeId, oldCell.item };

    // Validate move against game rules
    if (!rulesOfTheGame_->isMoveValid(gameState_, nextGameState_, player)) {
        emit invalidMove(player);
        return false;
    }

    // Update game state
    gameState_ = nextGameState_;
    updateVisuals();
    players_.next();

    emit playerMoved(player, nextPosition);

    // Check for game over
    PlayerEnums::PlayerPosition winner = rulesOfTheGame_->checkWin(gameState_);
    bool isFinish = rulesOfTheGame_->isGameOver(gameState_);
    setGameOpen(!isFinish);

    if (!gameOpen()) {
        emit gameOver(winner);
    }

    return true;
}

// --------------------------------------------
// Player Management
// --------------------------------------------

bool AbstractTableGame::addPlayer(Player *player) {
    Q_ASSERT(rulesOfTheGame_);
    if (rulesOfTheGame_->numberMaxPlayers() <= players_.size()) {
        return false;
    }

    if (players_.contains(player)) {
        return true;
    }

    player->setPlayerPosition(static_cast<PlayerEnums::PlayerPosition>(players_.size()));
    players_.append(player);
    return true;
}

bool AbstractTableGame::managePlayer(Player *player) {
    Q_ASSERT(rulesOfTheGame_);
    if (rulesOfTheGame_->numberMaxPlayers() <= players_.size()) {
        return false;
    }

    AbstractTableGame::addPlayer(player);
    if(!managedPlayers.contains(player)){
        managedPlayers.append(player);
    }
    return true;
}

// --------------------------------------------
// Mouse Interaction
// --------------------------------------------

void AbstractTableGame::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsScene::mousePressEvent(event);

    // Validate if game is open
    if (!gameOpen()) {
        return;
    }

    // Check if drag is allowed
    if (!rulesOfTheGame_ || !rulesOfTheGame_->isDragEnable()) {
        return;
    }

    // Check if it is your turn (managed Player)
    if(!managedPlayers.contains(players_.current())){
        return;
    }

    // Check if it is your item
    QPoint pressGameItem = mapPositionToGame(event->scenePos());
    PlayerEnums::PlayerPosition playerItem = gameState_.at(pressGameItem.ry(), pressGameItem.rx()).position;
    if(players_.current()->playerPosition() != playerItem){
        return;
    }

    // Get the item at the click position
    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
    if (item && isGamePiece(item)) {
        draggedItem_ = item;
        originalPos_ = item->pos();
        item->setCursor(QCursor(Qt::ClosedHandCursor));
    }
}

void AbstractTableGame::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsScene::mouseMoveEvent(event);

    if (draggedItem_) {
        // Get scene position and offset to center the item
        QPointF scenePos = event->scenePos();
        QRectF boundingRect = draggedItem_->boundingRect();
        QPointF offset(boundingRect.width() / 2, boundingRect.height() / 2);
        draggedItem_->setPos(scenePos - offset);
    }
}

void AbstractTableGame::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    QGraphicsScene::mouseReleaseEvent(event);

    if (!gameOpen()) {
        return;
    }

    // Check if it is your turn (managed Player)
    if(!managedPlayers.contains(players_.current())){
        return;
    }

    QPoint newPosition = mapPositionToGame(event->scenePos());
    if (newPosition != QPoint(-1,-1)) {
        QPoint previousPosition = newPosition;
        if (rulesOfTheGame_->isDragEnable()) {
            previousPosition = mapPositionToGame(originalPos_);
        }

        bool isMoveAccepted = playMove(players_.current(), previousPosition, newPosition);

        if (!isMoveAccepted) {
            qDebug() << "Invalid move!";
        }

        if (draggedItem_) {
            if (!isMoveAccepted) {
                draggedItem_->setPos(originalPos_);
            }
            draggedItem_->setCursor(QCursor(Qt::ArrowCursor));
            draggedItem_ = nullptr;
        }
    }
}

// --------------------------------------------
// Utility Functions
// --------------------------------------------

bool AbstractTableGame::isGamePiece(QGraphicsItem* item) const {
    return dynamic_cast<QGraphicsEllipseItem*>(item) || dynamic_cast<QGraphicsTextItem*>(item);
}

QPoint AbstractTableGame::mapPositionToGame(QPointF position) {
    for (int row = 0; row < cells_.rows(); ++row) {
        for (int col = 0; col < cells_.cols(); ++col) {
            QGraphicsRectItem* cell = cells_.at(row, col);
            if (cell && cell->contains(position)) {
                return QPoint(col, row);
            }
        }
    }
    return QPoint(-1, -1); // Invalid position
}
