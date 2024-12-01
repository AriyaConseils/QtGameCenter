#ifndef RANDOMBOT_H
#define RANDOMBOT_H

#include "AbstractBot/AbstractBot.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>
#include <vector>

class RandomBot : public AbstractBot {
    Q_OBJECT

public:
    explicit RandomBot(QObject* parent = nullptr) : AbstractBot(parent) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }

    QPoint play(const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& gameState) override {
        // Check if the game supports drag-and-drop
        if (gameScene->rulesOfTheGame()->isDragEnable()) {
            return playWithDrag(gameState);
        } else {
            return playWithoutDrag(gameState);
        }
    }


private:
    // Logic for games with drag-and-drop functionality
    QPoint playWithDrag(const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& gameState) {
        QVector<std::pair<int, int>> currentPositions; // Player's current piece positions
        QVector<QPoint> validMoves;                   // Valid moves for any piece

        // Find all current positions of the player's pieces
        for (int row = 0; row < gameState.rows(); ++row) {
            for (int col = 0; col < gameState.cols(); ++col) {
                if (gameState.at(row, col).position == this->playerPosition()) {
                    currentPositions.push_back({row, col});
                }
            }
        }

        std::random_device rd;
        std::default_random_engine rng(rd());

        // Use std::shuffle with the random number generator
        std::shuffle(currentPositions.begin(), currentPositions.end(), rng);


        // Define possible directions (diagonals and captures)
        static const QVector<std::pair<int, int>> directions = {
            {-2, -2}, {-2, 2}, {2, -2}, {2, 2}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
        };

        // Evaluate moves for each current position
        for (const auto& [currentRow, currentCol] : currentPositions) {
            for (const auto& [dx, dy] : directions) {
                int newRow = currentRow + dx;
                int newCol = currentCol + dy;

                // Check if the target position is within bounds and empty
                if (gameState.isValid(newRow, newCol) && gameState.at(newRow, newCol).position == PlayerEnums::Unknown) {
                    QMatrix<AbstractStrategyRulesGame::AtomicCellState> nextGameState = gameState;

                    // Simulate the move
                    nextGameState.at(newRow, newCol).position = this->playerPosition();
                    nextGameState.at(currentRow, currentCol).position = PlayerEnums::Unknown;

                    // Validate the move
                    if (gameScene->rulesOfTheGame()->isMoveValid(gameState, nextGameState, this)) {
                        validMoves.push_back(QPoint(newCol, newRow));
                    }
                }
            }
        }

        // If valid moves exist, play one
        if (!validMoves.isEmpty()) {
            for (const QPoint& move : validMoves) {
                for (const auto& [currentRow, currentCol] : currentPositions) {
                    QMatrix<AbstractStrategyRulesGame::AtomicCellState> nextGameState = gameState;

                    // Simulate the move
                    nextGameState.at(move.y(), move.x()).position = this->playerPosition();
                    nextGameState.at(currentRow, currentCol).position = PlayerEnums::Unknown;

                    // If the move is valid, play it
                    if (gameScene->rulesOfTheGame()->isMoveValid(gameState, nextGameState, this)) {
                        QPoint startingPosition = QPoint(currentCol, currentRow);
                        gameScene->playMove(this, startingPosition, move);
                        return move;
                    }
                }
            }
        }

        return QPoint(); // No valid move found
    }

    // Logic for games without drag-and-drop functionality (e.g., Tic-Tac-Toe)
    QPoint playWithoutDrag(const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& gameState) {
        QVector<QPoint> emptyPositions;

        // Find all empty positions
        for (int row = 0; row < gameState.rows(); ++row) {
            for (int col = 0; col < gameState.cols(); ++col) {
                if (gameState.at(row, col).position == PlayerEnums::Unknown) {
                    emptyPositions.push_back(QPoint(col, row));
                }
            }
        }

        QMatrix<AbstractStrategyRulesGame::AtomicCellState> nextGameState = gameState;
        QPoint choice;

        do {
            // If no empty positions remain
            if (emptyPositions.isEmpty()) {
                return QPoint();
            }

            // Randomly pick an empty position
            choice = emptyPositions[std::rand() % emptyPositions.size()];
            emptyPositions.removeOne(choice);

            // Simulate the move
            nextGameState = gameState;
            nextGameState.at(choice.y(), choice.x()).position = this->playerPosition();
        } while (!gameScene->rulesOfTheGame()->isMoveValid(gameState, nextGameState, this));

        // Play the chosen move
        gameScene->playMove(this, choice, choice);
        return choice;
    }
};

#endif // RANDOMBOT_H

