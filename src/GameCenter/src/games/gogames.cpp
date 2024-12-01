#include "GoGames.h"
#include <QFont>
#include <QPen>
#include <QGraphicsEllipseItem>
#include <QDebug>

// Constructor
GoGames::GoGames(QObject* parent)
    : AbstractTableGame(parent)
    , gridSize_(QSize(9, 9)) // Default board size for Go (9x9)
{
    rulesOfTheGame_ = new GoRules();
    drawGrid();
}

// --------------------------------------------
// Game Initialization
// --------------------------------------------

void GoGames::initializeGame()
{
    cells_.clear(); // Clear existing cells
    clear();        // Clear the scene
    drawGrid();     // Draw the initial grid
}

void GoGames::drawGrid()
{
    // Resize the cell matrix to match the grid dimensions
    cells_ = QMatrix<QGraphicsRectItem*>(gridSize_.height(), gridSize_.width(), nullptr);

    // Create the grid cells
    for (int row = 0; row < gridSize_.height(); ++row) {
        for (int col = 0; col < gridSize_.width(); ++col) {
            QGraphicsRectItem* cell = new QGraphicsRectItem(col * 50, row * 50, 50, 50); // Cell size 50x50 px
            cell->setBrush(Qt::white);
            cell->setPen(QPen(Qt::black));
            addItem(cell); // Add cell to the scene

            // Store the cell in the matrix
            cells_.at(row, col) = cell;
        }
    }
}

// --------------------------------------------
// Update Visuals
// --------------------------------------------

void GoGames::updateVisuals()
{
    // Remove all existing items from the scene (grid and stones)
    for (auto item : items()) {
        removeItem(item); // Remove item from the scene
        delete item;      // Free memory
    }

    cells_.clear(nullptr); // Reset the cell matrix

    // Redraw the grid
    drawGrid();

    // Add stones to match the current game state
    for (int row = 0; row < gameState_.rows(); ++row) {
        for (int col = 0; col < gameState_.cols(); ++col) {
            PlayerEnums::PlayerPosition currentPlayer = gameState_.at(row, col).position;
            if (currentPlayer != PlayerEnums::Unknown) {
                // Create a graphical representation for the stone
                QGraphicsEllipseItem* stone = new QGraphicsEllipseItem(0, 0, 40, 40); // Circular stone

                // Set the stone color
                stone->setBrush((currentPlayer == players_.at(0)->playerPosition()) ? Qt::black : Qt::white);
                stone->setPen(QPen(Qt::black));

                // Set absolute position in the scene
                stone->setPos(col * 50 + 5, row * 50 + 5);

                // Add the stone to the scene
                addItem(stone);
            }
        }
    }
}
