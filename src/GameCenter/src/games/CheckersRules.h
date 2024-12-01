#ifndef CHECKERSRULES_H
#define CHECKERSRULES_H

#include "AbstractGame/AbstractStrategyRulesGame.h"
#include "AbstractGame/Player.h"
#include <QFile>

class CheckersRules : public AbstractStrategyRulesGame {
public:
    explicit CheckersRules() = default;
    ~CheckersRules() override = default;

    // Retourne le nom du jeu
    QString gameName() const override { return QStringLiteral("Checkers"); }

    // Retourne les règles du jeu
    QString gameRules() const override {
        QFile file(":/docs/rules of the game/Checkers.html");
        return file.open(QIODevice::ReadOnly | QIODevice::Text) ? QTextStream(&file).readAll() : "Error: Unable to load game rules.";
    }

    int numberMinPlayers() const override { return 2; }
    int numberMaxPlayers() const override { return 2; }
    bool isDragEnable() const override { return true; }

    QMatrix<AbstractStrategyRulesGame::AtomicCellState> initStatePlayers(int gridX, int gridY) const override {
        QMatrix<AbstractStrategyRulesGame::AtomicCellState> state(gridX, gridY, { PlayerEnums::Unknown, 0 , nullptr});

        for (int row = 0; row < gridX; ++row) {
            for (int col = 0; col < gridY; ++col) {
                if ((row + col) % 2 == 1) { // Case noire
                    if (row < 3) {
                        state.at(row, col) = { PlayerEnums::Player1, 1, nullptr }; // Pion du joueur 1
                    } else if (row >= gridX - 3) {
                        state.at(row, col) = { PlayerEnums::Player2, 1 , nullptr }; // Pion du joueur 2
                    }
                }
            }
        }
        return state;
    }

    bool isMoveValid(const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& beforeState, QMatrix<AbstractStrategyRulesGame::AtomicCellState>& afterState, Player *player) override {
        if (beforeState.rows() != afterState.rows() || beforeState.cols() != afterState.cols()) {
            return false;
        }

        int changes = 0;
        int startRow = -1, startCol = -1;
        int endRow = -1, endCol = -1;

        for (int row = 0; row < beforeState.rows(); ++row) {
            for (int col = 0; col < beforeState.cols(); ++col) {
                if (beforeState.at(row, col).position != afterState.at(row, col).position) {
                    if (beforeState.at(row, col).position == player->playerPosition()) {
                        startRow = row;
                        startCol = col;
                    } else if (afterState.at(row, col).position == player->playerPosition()) {
                        endRow = row;
                        endCol = col;
                    } else {
                        return false; // Mouvement invalide
                    }
                    ++changes;
                }
            }
        }

        // Vérification des mouvements simples ou captures
        if (changes == 2) {
            if (isSimpleMoveValid(beforeState, startRow, startCol, endRow, endCol, player)) {
                // Promotion en dame si le pion atteint la dernière rangée
                promoteToKing(afterState, endRow, endCol, player);
                return true;
            } else if (isCaptureMoveValid(beforeState, startRow, startCol, endRow, endCol, player)) {
                // Retirer la pièce capturée
                int midRow = (startRow + endRow) / 2;
                int midCol = (startCol + endCol) / 2;
                afterState.at(midRow, midCol) = { PlayerEnums::Unknown, 0, nullptr };

                // Promotion en dame si le pion atteint la dernière rangée
                promoteToKing(afterState, endRow, endCol, player);
                return true;
            }
        }

        return false;
    }

    PlayerEnums::PlayerPosition checkWin(const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& gameState) const override {
        bool player1Exists = false;
        bool player2Exists = false;

        for (int row = 0; row < gameState.rows(); ++row) {
            for (int col = 0; col < gameState.cols(); ++col) {
                if (gameState.at(row, col).position == PlayerEnums::Player1) {
                    player1Exists = true;
                }
                if (gameState.at(row, col).position == PlayerEnums::Player2) {
                    player2Exists = true;
                }
            }
        }

        if (!player1Exists) {
            return PlayerEnums::Player2;
        }
        if (!player2Exists) {
            return PlayerEnums::Player1;
        }

        return PlayerEnums::Unknown;
    }

    bool isGameOver(const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& gameState) const override {
        return checkWin(gameState) != PlayerEnums::Unknown;
    }

private:
    bool isSimpleMoveValid(const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& state, int startRow, int startCol, int endRow, int endCol, Player *player) const {
        int direction = (player->playerPosition() == PlayerEnums::Player1) ? 1 : -1;

        // Vérifie les mouvements en diagonale d'une seule case
        if (std::abs(endRow - startRow) == 1 && std::abs(endCol - startCol) == 1) {
            if (state.at(endRow, endCol).position == PlayerEnums::Unknown &&
                ((endRow - startRow) == direction || state.at(startRow, startCol).typeId == 2)) {
                return true;
            }
        }
        return false;
    }

    bool isCaptureMoveValid(const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& state, int startRow, int startCol, int endRow, int endCol, Player *player) const {
        int direction = (player->playerPosition() == PlayerEnums::Player1) ? 1 : -1;

        // Vérifie les captures en sautant une case
        if (std::abs(endRow - startRow) == 2 && std::abs(endCol - startCol) == 2) {
            int midRow = (startRow + endRow) / 2;
            int midCol = (startCol + endCol) / 2;

            if (state.at(midRow, midCol).position != PlayerEnums::Unknown &&
                state.at(midRow, midCol).position != player->playerPosition() &&
                state.at(endRow, endCol).position == PlayerEnums::Unknown &&
                ((endRow - startRow) / 2 == direction || state.at(startRow, startCol).typeId == 2)) {
                return true;
            }
        }
        return false;
    }

    void promoteToKing(QMatrix<AbstractStrategyRulesGame::AtomicCellState>& state, int row, int col, Player *player) const {
        int lastRow = (player->playerPosition() == PlayerEnums::Player1) ? state.rows() - 1 : 0;
        if (row == lastRow) {
            state.at(row, col).typeId = 2; // Promotion en dame
        }
    }
};

#endif // CHECKERSRULES_H
