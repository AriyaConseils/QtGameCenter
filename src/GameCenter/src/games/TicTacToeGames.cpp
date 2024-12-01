#include "TicTacToeGames.h"
#include <QFont>
#include <QPen>
#include <QDebug>

// --------------------------------------------
// Constructor
// --------------------------------------------

TicTacToeGames::TicTacToeGames(QObject* parent)
    : AbstractTableGame(parent)
    , gridSize_(QSize(4, 4)) // Default grid size for Tic-Tac-Toe
{
    rulesOfTheGame_ = new TicTacToeRules();
    drawGrid(); // Initial drawing of the grid
}

// --------------------------------------------
// Game Initialization
// --------------------------------------------

void TicTacToeGames::initializeGame()
{
    // Remove all existing graphical elements
    for (int row = 0; row < cells_.rows(); ++row) {
        for (int col = 0; col < cells_.cols(); ++col) {
            if (cells_.at(row, col)) {
                removeItem(cells_.at(row, col)); // Remove each cell
            }
        }
    }
    cells_.clear(nullptr);
    clear();        // Clear the scene
    drawGrid();     // Draw the grid again
}

void TicTacToeGames::drawGrid()
{
    // Resize the cell matrix to match the grid dimensions
    cells_ = QMatrix<QGraphicsRectItem*>(gridSize_.height(), gridSize_.width(), nullptr);

    // Create the grid cells
    for (int row = 0; row < gridSize_.height(); ++row) {
        for (int col = 0; col < gridSize_.width(); ++col) {
            QGraphicsRectItem* cell = new QGraphicsRectItem(col * 100, row * 100, 100, 100);
            cell->setBrush(Qt::white);
            cell->setPen(QPen(Qt::black));
            addItem(cell);             // Add cell to the scene
            cells_.at(row, col) = cell; // Store the cell in the matrix
        }
    }
}

// --------------------------------------------
// Utility Methods
// --------------------------------------------

QString TicTacToeGames::playerToString(PlayerEnums::PlayerPosition player) const
{
    // Return "X" for the first player and "O" for the second
    return (players_.at(0)->playerPosition() == player) ? "X" : "O";
}

// --------------------------------------------
// Update Visuals
// --------------------------------------------

void TicTacToeGames::updateVisuals()
{
    // Remove all existing graphical elements
    initializeGame();

    // Add symbols to represent the current game state
    for (int row = 0; row < gameState_.rows(); ++row) {
        for (int col = 0; col < gameState_.cols(); ++col) {
            PlayerEnums::PlayerPosition currentPlayer = gameState_.at(row, col).position;
            if (currentPlayer != PlayerEnums::Unknown) {
                // Create a text item for the player's symbol
                QString symbol = playerToString(currentPlayer);
                QGraphicsTextItem* text = addText(symbol, QFont("Arial", 36));

                // Set the text color based on the player
                text->setDefaultTextColor((currentPlayer == players_.at(0)->playerPosition()) ? Qt::black : Qt::red);

                // Position the text within the corresponding cell
                QGraphicsRectItem* cell = cells_.at(row, col);
                if (cell) {
                    text->setPos(cell->rect().x() + 30, cell->rect().y() + 20);
                }
            }
        }
    }
}
