#include "CheckersGame.h"
#include <QFont>
#include <QPen>
#include <QBrush>
#include <QGraphicsEllipseItem>
#include <QDebug>

// Constructor
CheckersGame::CheckersGame(QObject* parent)
    : AbstractTableGame(parent)
    , gridSize_(QSize(8, 8)) // Standard checkers board size
{
    // Install rules for the checkers game
    rulesOfTheGame_ = new CheckersRules();

    connect(this, &AbstractTableGame::gameOpenChanged, this, [this]() {
        if (gameOpen()) {
            updateVisuals();
        }
    });

    drawGrid();
}

// --------------------------------------------
// Game Initialization
// --------------------------------------------

void CheckersGame::initializeGame() {
    cells_.clear();
    clear();
    drawGrid();
}

void CheckersGame::drawGrid() {
    // Set up the board with alternating light and dark squares
    cells_ = QMatrix<QGraphicsRectItem*>(gridSize_.height(), gridSize_.width(), nullptr);

    for (int row = 0; row < gridSize_.height(); ++row) {
        for (int col = 0; col < gridSize_.width(); ++col) {
            QGraphicsRectItem* cell = new QGraphicsRectItem(col * 100, row * 100, 100, 100);

            // Alternate colors between dark gray and light gray
            cell->setBrush(((row + col) % 2 == 0) ? Qt::lightGray : Qt::darkGray);
            cell->setPen(QPen(Qt::black));
            addItem(cell);

            cells_.at(row, col) = cell; // Store cell in the matrix
        }
    }
}

// --------------------------------------------
// Update Visuals
// --------------------------------------------

void CheckersGame::updateVisuals() {
    // Loop through the game state to add or update pieces
    for (int row = 0; row < gameState_.rows(); ++row) {
        for (int col = 0; col < gameState_.cols(); ++col) {
            AbstractStrategyRulesGame::AtomicCellState cellState = gameState_.at(row, col);

            if (cellState.position != PlayerEnums::Unknown) {
                // If the piece doesn't already exist, create it
                if (cellState.item == nullptr) {
                    cellState.item = new QGraphicsEllipseItem(0, 0, 80, 80);
                    addItem(cellState.item);
                    gameState_[row][col].item = cellState.item; // Update game state with the new item
                }

                // Set the position of the piece
                cellState.item->setPos(col * 100 + 10, row * 100 + 10);

                // Set the color of the piece
                QBrush brush((cellState.position == PlayerEnums::Player1) ? Qt::black : Qt::red);
                cellState.item->setBrush(brush);

                // Add a golden border if it's a king
                if (cellState.typeId == 2) {
                    QPen pen(Qt::yellow);
                    pen.setWidth(4);
                    cellState.item->setPen(pen);
                } else {
                    cellState.item->setPen(Qt::NoPen); // No border for regular pieces
                }
            }
        }
    }

    // Clean up any orphaned graphical items
    cleanUpGame();
}

// --------------------------------------------
// Cleanup Logic
// --------------------------------------------

void CheckersGame::cleanUpGame() {
    // Remove graphical items that are no longer in the game state
    QList<QGraphicsItem*> allItems = items();
    for (QGraphicsItem* item : allItems) {
        if (QGraphicsEllipseItem* ellipse = dynamic_cast<QGraphicsEllipseItem*>(item)) {
            bool found = false;

            // Check if the item is still in the game state
            for (int row = 0; row < gameState_.rows(); ++row) {
                for (int col = 0; col < gameState_.cols(); ++col) {
                    if (gameState_.at(row, col).item == ellipse) {
                        found = true;
                        break;
                    }
                }
                if (found) break;
            }

            // If not found, remove it from the scene and delete it
            if (!found) {
                removeItem(ellipse);
                delete ellipse;
            }
        }
    }
}
