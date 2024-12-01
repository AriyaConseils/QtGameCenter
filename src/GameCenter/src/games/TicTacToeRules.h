#ifndef TICTACTOERULES_H
#define TICTACTOERULES_H

#include "AbstractGame/AbstractStrategyRulesGame.h"
#include "AbstractGame/Player.h"
#include <QFile>

class TicTacToeRules : public AbstractStrategyRulesGame {
public:
    explicit TicTacToeRules() = default;
    ~TicTacToeRules() override = default;

    // Retourne le nom du jeu
    QString gameName() const override { return QStringLiteral("TicTacToe"); }

    // Retourne les règles du jeu
    QString gameRules() const override {
        QFile file(":/docs/rules of the game/TicTacToe.html");
        return file.open(QIODevice::ReadOnly | QIODevice::Text) ? QTextStream(&file).readAll() : "Error: Unable to load game rules.";
    }

    // Minimum number of players required
    int numberMinPlayers() const override { return 2; }

    // Maximum number of players allowed
    int numberMaxPlayers() const override { return 2; }

    // Dragging is disabled in Tic-Tac-Toe
    bool isDragEnable() const override { return false; }

    // Initialize the game state
    QMatrix<AbstractStrategyRulesGame::AtomicCellState> initStatePlayers(int gridX, int gridY) const override {
        return QMatrix<AbstractStrategyRulesGame::AtomicCellState>(gridX, gridY, { PlayerEnums::Unknown, 0 });
    }

    // Validate a player's move
    bool isMoveValid(const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& beforeState,
                     QMatrix<AbstractStrategyRulesGame::AtomicCellState>& afterState,
                     Player *player) override {
        // Ensure the dimensions match
        if (beforeState.rows() != afterState.rows() || beforeState.cols() != afterState.cols()) {
            return false;
        }

        int changes = 0;
        for (int row = 0; row < beforeState.rows(); ++row) {
            for (int col = 0; col < beforeState.cols(); ++col) {
                if (beforeState.at(row, col).position != afterState.at(row, col).position) {
                    // Ensure the move involves only one empty cell
                    if (beforeState.at(row, col).position != PlayerEnums::Unknown ||
                        afterState.at(row, col).position != player->playerPosition()) {
                        return false; // Invalid move
                    }
                    ++changes;
                }
            }
        }

        return changes == 1; // Exactly one cell should change
    }

    // Check if there's a winner
    PlayerEnums::PlayerPosition checkWin(const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& gameState) const override {
        int gridSize = gameState.rows(); // Grid is square, rows == cols

        // Check rows
        for (int row = 0; row < gridSize; ++row) {
            PlayerEnums::PlayerPosition firstPlayer = gameState.at(row, 0).position;
            if (firstPlayer != PlayerEnums::Unknown) {
                bool rowWin = true;
                for (int col = 1; col < gridSize; ++col) {
                    if (gameState.at(row, col).position != firstPlayer) {
                        rowWin = false;
                        break;
                    }
                }
                if (rowWin) {
                    return firstPlayer; // Winner in this row
                }
            }
        }

        // Check columns
        for (int col = 0; col < gridSize; ++col) {
            PlayerEnums::PlayerPosition firstPlayer = gameState.at(0, col).position;
            if (firstPlayer != PlayerEnums::Unknown) {
                bool colWin = true;
                for (int row = 1; row < gridSize; ++row) {
                    if (gameState.at(row, col).position != firstPlayer) {
                        colWin = false;
                        break;
                    }
                }
                if (colWin) {
                    return firstPlayer; // Winner in this column
                }
            }
        }

        // Check main diagonal
        PlayerEnums::PlayerPosition mainDiagonalPlayer = gameState.at(0, 0).position;
        if (mainDiagonalPlayer != PlayerEnums::Unknown) {
            bool mainDiagonalWin = true;
            for (int i = 1; i < gridSize; ++i) {
                if (gameState.at(i, i).position != mainDiagonalPlayer) {
                    mainDiagonalWin = false;
                    break;
                }
            }
            if (mainDiagonalWin) {
                return mainDiagonalPlayer; // Winner in main diagonal
            }
        }

        // Check secondary diagonal
        PlayerEnums::PlayerPosition secondaryDiagonalPlayer = gameState.at(0, gridSize - 1).position;
        if (secondaryDiagonalPlayer != PlayerEnums::Unknown) {
            bool secondaryDiagonalWin = true;
            for (int i = 1; i < gridSize; ++i) {
                if (gameState.at(i, gridSize - 1 - i).position != secondaryDiagonalPlayer) {
                    secondaryDiagonalWin = false;
                    break;
                }
            }
            if (secondaryDiagonalWin) {
                return secondaryDiagonalPlayer; // Winner in secondary diagonal
            }
        }

        // No winner
        return PlayerEnums::Unknown;
    }

    // Check if the game is over
    bool isGameOver(const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& gameState) const override {
        // Check if a player has already won
        if (checkWin(gameState) != PlayerEnums::Unknown) {
            return true;
        }

        // Track whether there are any empty cells
        bool hasEmptyCells = false;

        // Iterate over the game state
        for (int row = 0; row < gameState.rows(); ++row) {
            for (int col = 0; col < gameState.cols(); ++col) {
                if (gameState.at(row, col).position == PlayerEnums::Unknown) {
                    hasEmptyCells = true;

                    // Simulate moves for both players at this empty cell
                    for (auto player : {PlayerEnums::Player1, PlayerEnums::Player2}) {
                        QMatrix<AbstractStrategyRulesGame::AtomicCellState> simulatedState = gameState;
                        simulatedState.at(row, col).position = player;

                        // If this move leads to a win for any player, game is not over
                        if (checkWin(simulatedState) == player) {
                            return false; // A player can still win
                        }
                    }
                }
            }
        }

        // If there are no empty cells or no player can win, the game is over
        return !hasEmptyCells;
    }

};

#endif // TICTACTOERULES_H
