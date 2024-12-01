#ifndef GORULES_H
#define GORULES_H

#include "AbstractGame/AbstractStrategyRulesGame.h"
#include "framework/types/QMatrix.h"
#include <stack>
#include <set>
#include <vector>
#include <QFile>

class GoRules : public AbstractStrategyRulesGame {
public:
    explicit GoRules() = default;
    ~GoRules() override = default;

    // Retourne le nom du jeu
    QString gameName() const override { return QStringLiteral("Go"); }

    // Retourne les règles du jeu
    QString gameRules() const override {
        QFile file(":/docs/rules of the game/Go.html");
        return file.open(QIODevice::ReadOnly | QIODevice::Text) ? QTextStream(&file).readAll() : "Error: Unable to load game rules.";
    }

    int numberMinPlayers() const override { return 2; }
    int numberMaxPlayers() const override { return 2; }
    bool isDragEnable() const override { return false; }

    QMatrix<AbstractStrategyRulesGame::AtomicCellState> initStatePlayers(int gridX, int gridY) const override {
        return QMatrix<AbstractStrategyRulesGame::AtomicCellState>(gridX, gridY, { PlayerEnums::Unknown, 0 });
    }

    bool isMoveValid(const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& beforeState, QMatrix<AbstractStrategyRulesGame::AtomicCellState>& afterState, Player *player) override {
        if (!validateMatrixDimensions(beforeState, afterState)) {
            return false;
        }

        int changes = 0;
        int placedRow = -1;
        int placedCol = -1;

        for (int row = 0; row < beforeState.rows(); ++row) {
            for (int col = 0; col < beforeState.cols(); ++col) {
                if (beforeState.at(row, col).position != afterState.at(row, col).position) {
                    // Vérifie qu'une seule case vide est jouée par le joueur
                    if (beforeState.at(row, col).position != PlayerEnums::Unknown || afterState.at(row, col).position != player->playerPosition()) {
                        return false; // Modification invalide
                    }
                    placedRow = row;
                    placedCol = col;
                    ++changes;
                }
            }
        }

        if (changes != 1) return false; // Plus d'une modification détectée

        // Simuler le placement pour vérifier les libertés
        afterState.at(placedRow, placedCol).position = player->playerPosition();

        // Vérifier si le groupe du joueur placé a des libertés après capture des ennemis adjacents
        QMatrix<AbstractStrategyRulesGame::AtomicCellState> simulatedState = afterState;
        captureStones(simulatedState, player->playerPosition());

        QMatrix<bool> visited(simulatedState.rows(), simulatedState.cols(), false);
        if (!hasLiberties(simulatedState, placedRow, placedCol, player->playerPosition(), visited)) {
            return false; // Le mouvement placerait un groupe sans liberté (suicide)
        }

        // Si valide, capturer les groupes adverses dans l'état final
        captureStones(afterState, player->playerPosition());
        return true;
    }

    PlayerEnums::PlayerPosition checkWin(const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& gameState) const override {
        return Player().playerPosition(); // Simplification : aucun calcul des territoires
    }

    bool isGameOver(const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& gameState) const override {
        for (int row = 0; row < gameState.rows(); ++row) {
            for (int col = 0; col < gameState.cols(); ++col) {
                if (gameState.at(row, col).position == PlayerEnums::Unknown) {
                    return false; // Plateau encore disponible
                }
            }
        }
        return true;
    }

private:
    bool validateMatrixDimensions(const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& beforeState, const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& afterState) const {
        return beforeState.rows() == afterState.rows() && beforeState.cols() == afterState.cols() && beforeState.rows() > 0;
    }


    void captureStones(QMatrix<AbstractStrategyRulesGame::AtomicCellState>& gameState, PlayerEnums::PlayerPosition playerPosition) {
        QMatrix<bool> visited(gameState.rows(), gameState.cols(), false);

        for (int row = 0; row < gameState.rows(); ++row) {
            for (int col = 0; col < gameState.cols(); ++col) {
                PlayerEnums::PlayerPosition stone = gameState.at(row, col).position;
                if (stone != PlayerEnums::Unknown && stone != playerPosition && !visited.at(row, col)) {
                    if (!hasLiberties(gameState, row, col, stone, visited)) {
                        removeGroup(gameState, row, col, stone);
                    }
                }
            }
        }
    }


    bool hasLiberties(const QMatrix<AbstractStrategyRulesGame::AtomicCellState>& gameState, int startRow, int startCol, PlayerEnums::PlayerPosition stone, QMatrix<bool>& visited) const {
        std::stack<std::pair<int, int>> positions;
        positions.push({startRow, startCol});

        while (!positions.empty()) {
            auto [row, col] = positions.top();
            positions.pop();

            if (!gameState.isValid(row, col) || visited.at(row, col)) {
                continue;
            }
            visited.at(row, col) = true;

            if (gameState.at(row, col).position == PlayerEnums::Unknown) {
                return true; // Liberté trouvée
            }

            if (gameState.at(row, col).position != stone) {
                continue;
            }

            // Parcourir les voisins
            static const std::vector<std::pair<int, int>> directions = {
                {0, -1}, {0, 1}, {-1, 0}, {1, 0}
            };

            for (const auto& [dx, dy] : directions) {
                int newRow = row + dx;
                int newCol = col + dy;
                if (gameState.isValid(newRow, newCol) && !visited.at(newRow, newCol)) {
                    positions.push({newRow, newCol});
                }
            }
        }

        return false; // Pas de libertés trouvées
    }

    void removeGroup(QMatrix<AbstractStrategyRulesGame::AtomicCellState>& gameState, int startRow, int startCol, PlayerEnums::PlayerPosition stone) {
        std::stack<std::pair<int, int>> positions;
        positions.push({startRow, startCol});

        while (!positions.empty()) {
            auto [row, col] = positions.top();
            positions.pop();

            if (!gameState.isValid(row, col) || gameState.at(row, col).position != stone) {
                continue;
            }

            gameState.at(row, col).position = PlayerEnums::Unknown; // Supprimer la pierre

            // Directions pour parcourir les voisins
            static const std::vector<std::pair<int, int>> directions = {
                {0, -1}, {0, 1}, {-1, 0}, {1, 0}
            };

            for (const auto& [dx, dy] : directions) {
                int newRow = row + dx;
                int newCol = col + dy;
                if (gameState.isValid(newRow, newCol) && gameState.at(newRow, newCol).position == stone) {
                    positions.push({newRow, newCol});
                }
            }
        }
    }
};

#endif // GORULES_H
