#ifndef ABSTRACTSTRATEGYRULESGAME_H
#define ABSTRACTSTRATEGYRULESGAME_H

#include "framework/types/QMatrix.h"
#include "Player.h"

class QAbstractGraphicsShapeItem;

class AbstractStrategyRulesGame {

public:
    typedef struct {
        PlayerEnums::PlayerPosition position;
        int typeId;
        QAbstractGraphicsShapeItem *item;
    } AtomicCellState;

    virtual ~AbstractStrategyRulesGame() {}

    // Retourne le nom du jeu
    virtual QString gameName() const = 0;

    // Retourne les règles du jeu
    virtual QString gameRules() const = 0;

    // Retourne le nombre de joueurs dans le jeu
    virtual int numberMaxPlayers() const = 0;

    virtual int numberMinPlayers() const = 0;

    virtual QMatrix<AtomicCellState> initStatePlayers(int gridX, int gridY) const = 0;

    virtual bool isDragEnable() const = 0;

    // Vérifie si un mouvement est valide entre deux états
    virtual bool isMoveValid(const QMatrix<AtomicCellState> &beforeState, QMatrix<AtomicCellState> &afterState, Player *player) = 0;

    // Vérifie si un joueur a gagné dans l'état actuel
    virtual PlayerEnums::PlayerPosition checkWin(const QMatrix<AtomicCellState> &gameState) const = 0;

    // Vérifie si le jeu est terminé
    virtual bool isGameOver(const QMatrix<AtomicCellState> &gameState) const = 0;
};

#endif // ABSTRACTSTRATEGYRULESGAME_H
