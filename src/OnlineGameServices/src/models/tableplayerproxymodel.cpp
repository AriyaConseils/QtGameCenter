#include "TablePlayerProxyModel.h"
#include <QDebug>

TablePlayerProxyModel::TablePlayerProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent) {}

// Ajouter un playerId au filtre
void TablePlayerProxyModel::addPlayerId(const QString& playerId) {
    filteredPlayerIds_.insert(playerId);
    invalidateFilter(); // Recalcule le filtre
}

// Supprimer un playerId du filtre
void TablePlayerProxyModel::removePlayerId(const QString& playerId) {
    filteredPlayerIds_.remove(playerId);
    invalidateFilter(); // Recalcule le filtre
}

// Effacer tous les playerId du filtre
void TablePlayerProxyModel::clearPlayerIds() {
    filteredPlayerIds_.clear();
    invalidateFilter(); // Recalcule le filtre
}

// Méthode pour le tri personnalisé (par exemple, tri par position des joueurs)
bool TablePlayerProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const {
    auto playerModel = qobject_cast<PlayerModel*>(sourceModel());
    if (!playerModel) return false;

    QString leftPlayerId = left.data(Qt::UserRole).toString();
    QString rightPlayerId = right.data(Qt::UserRole).toString();

    Player* leftPlayer = playerModel->getPlayer(leftPlayerId);
    Player* rightPlayer = playerModel->getPlayer(rightPlayerId);

    if (!leftPlayer || !rightPlayer) return false;

    // Tri par position
    return leftPlayer->playerPosition() < rightPlayer->playerPosition();
}

// Méthode pour filtrer les lignes du modèle source
bool TablePlayerProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
    auto playerModel = qobject_cast<PlayerModel*>(sourceModel());
    if (!playerModel) return false;

    QModelIndex index = playerModel->index(sourceRow, 0, sourceParent);
    QString playerId = index.data(Qt::UserRole + 5).toString();

    // Afficher uniquement les joueurs dont le playerId est dans le filtre
    bool matchesFilter = filteredPlayerIds_.contains(playerId);


    return matchesFilter;
}
