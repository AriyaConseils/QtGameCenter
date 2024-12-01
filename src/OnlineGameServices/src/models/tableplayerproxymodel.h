#ifndef TABLEPLAYERPROXYMODEL_H
#define TABLEPLAYERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include "PlayerModel.h"

class TablePlayerProxyModel : public QSortFilterProxyModel {
    Q_OBJECT

public:
    explicit TablePlayerProxyModel(QObject* parent = nullptr);

    // Ajouter un playerId à afficher
    void addPlayerId(const QString& playerId);

    // Supprimer un playerId
    void removePlayerId(const QString& playerId);

    // Effacer le filtre
    void clearPlayerIds();

protected:
    // Méthodes redéfinies pour le tri et le filtrage
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    QSet<QString> filteredPlayerIds_; // Liste des playerId autorisés
};

#endif // TABLEPLAYERPROXYMODEL_H
