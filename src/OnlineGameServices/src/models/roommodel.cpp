#include "RoomModel.h"

RoomModel::RoomModel(QObject *parent)
    : QAbstractItemModel(parent) {
    QVariantList rootData;
    rootData << "Game Room";
    rootItem = new TreeItem(rootData);
}

RoomModel::~RoomModel() {
    delete rootItem;
}

QModelIndex RoomModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem = parent.isValid() ? static_cast<TreeItem *>(parent.internalPointer()) : rootItem;

    TreeItem *childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex RoomModel::parent(const QModelIndex &index) const {
    // Vérifier si l'index est valide
    if (!index.isValid()) {
        return QModelIndex();  // Si l'index est invalide, on retourne un index invalide
    }

    // Récupérer l'élément de type TreeItem correspondant à cet index
    TreeItem *childItem = static_cast<TreeItem *>(index.internalPointer());

    // Vérifier si l'élément a un parent
    TreeItem *parentItem = childItem ? childItem->parentItem() : nullptr;

    // Si le parent existe, créer un index pour le parent et le retourner
    if (parentItem) {
        // On renvoie un index correspondant au parent de l'élément courant
        return createIndex(parentItem->row(), 0, parentItem);
    }

    // Si l'élément n'a pas de parent (c'est la racine), on retourne un index invalide
    return QModelIndex();
}


int RoomModel::rowCount(const QModelIndex &parent) const {
    TreeItem *parentItem = parent.isValid() ? static_cast<TreeItem *>(parent.internalPointer()) : rootItem;
    return parentItem->childCount();
}

int RoomModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return rootItem->columnCount();
}

QVariant RoomModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    TreeItem *item = static_cast<TreeItem *>(index.internalPointer());

    if (role == Qt::DisplayRole) {
        // Reconstruire dynamiquement le message pour une salle
        QJsonObject roomInfo = item->data(0).toJsonObject();
        if (item->parentItem() == rootItem) { // C'est une salle
            QString gameName = roomInfo["game"].toString();
            QString firstPlayerName = roomInfo["players"].toArray().first()["firstName"].toString();
            return QString("%1 room with: %2").arg(gameName, firstPlayerName);
        } else {
            QString firstName = roomInfo["firstName"].toString();
            QString lastName = roomInfo["lastName"].toString();
            return QString("%1 %2").arg(firstName, lastName);
        }
    }

    return QVariant();
}


void RoomModel::addRoom(const QJsonObject &roomInfo) {
    qDebug() << "Adding new room with UUID:" << roomInfo["uuid"].toString();

    // Démarre l'insertion de nouvelles lignes dans le modèle
    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    // Créer un nouvel objet TreeItem pour la chambre
    auto roomItem = std::make_unique<TreeItem>(QVariantList{roomInfo}, rootItem);

    // Ajouter les joueurs comme enfants de la chambre
    QJsonArray players = roomInfo["players"].toArray();
    for (const QJsonValue &player : players) {
        QJsonObject playerInfo = player.toObject();
        auto playerItem = std::make_unique<TreeItem>(QVariantList{playerInfo}, roomItem.get());
        roomItem->appendChild(std::move(playerItem)); // Ajout du joueur à la chambre
    }

    // Ajouter l'élément de chambre à la liste des items
    rootItem->appendChild(std::move(roomItem));

    // Fin de l'insertion de lignes dans le modèle
    endInsertRows();
}



void RoomModel::updateRoom(const QString &uuid, const QJsonObject &roomInfo) {
    // Rechercher la room par UUID
    TreeItem *roomItem = findRoomByUUID(uuid);
    if (!roomItem) {
        qDebug() << "Room with UUID:" << uuid << "not found. Cannot update.";
        return;
    }

    // Début de la mise à jour des données
    qDebug() << "Updating room with UUID:" << uuid;

    // Suppression des enfants de roomItem (comme les joueurs, par exemple)
    int childCount = roomItem->childCount();
    if (childCount > 0) {
        beginRemoveRows(QModelIndex(), 0, childCount - 1);  // Supprimer toutes les lignes existantes
        roomItem->clearChildren();  // Effacer les enfants du TreeItem
        endRemoveRows();  // Fin de la suppression des lignes
    }

    // Mise à jour des données de la pièce
    QVariantList roomData;
    roomData << roomInfo;  // Ajouter les nouvelles données de la pièce
    roomItem->setItemData(roomData);  // Mettre à jour l'élément principal avec les nouvelles données

    // Ajouter les joueurs comme enfants (après mise à jour des informations de la pièce)
    QJsonArray players = roomInfo["players"].toArray();
    beginInsertRows(QModelIndex(), roomItem->childCount(), roomItem->childCount() + players.size() - 1);

    for (const QJsonValue &player : players) {
        QJsonObject playerInfo = player.toObject();
        auto playerItem = std::make_unique<TreeItem>(QVariantList{playerInfo}, roomItem);
        roomItem->appendChild(std::move(playerItem)); // Ajout du joueur à la chambre
    }


    endInsertRows();  // Fin de l'insertion des nouvelles lignes
}



void RoomModel::addOrUpdateRoom(const QJsonObject &roomInfo) {
    QString uuid = roomInfo["uuid"].toString();
    bool active = roomInfo["active"].toBool();

    TreeItem *existingRoom = findRoomByUUID(uuid);

    if (existingRoom && active) {
        qDebug() << "Room with UUID:" << uuid << "exists. Updating.";
        updateRoom(uuid, roomInfo);
    } else {
        qDebug() << "Room with UUID:" << uuid << "does not exist. Adding.";
        addRoom(roomInfo);
    }
}

TreeItem *RoomModel::findRoomByUUID(const QString &uuid) const {
    for (int i = 0; i < rootItem->childCount(); ++i) {
        TreeItem *room = rootItem->child(i);
        QJsonObject roomUuid = room->data(0).toJsonObject();
        if (roomUuid["uuid"].toString() == uuid) {
            qDebug() << "Found room with UUID:" << uuid;
            return room;
        }
    }
    qDebug() << "Room with UUID:" << uuid << "not found.";
    return nullptr;
}


void RoomModel::removeRoom(const QString &uuid)
{
    // Trouver la salle correspondant à l'UUID
    TreeItem *roomItem = findRoomByUUID(uuid);
    if (!roomItem) {
        qDebug() << "Room with UUID:" << uuid << "not found. Cannot remove.";
        return;
    }

    // Obtenir l'élément parent (dans ce cas, rootItem)
    TreeItem *parentItem = roomItem->parentItem();
    if (!parentItem) {
        qDebug() << "Parent item for room with UUID:" << uuid << "not found. Cannot remove.";
        return;
    }

    // Trouver l'index de la salle parmi les enfants du parent
    int row = roomItem->row();
    if (row < 0 || row >= parentItem->childCount()) {
        qDebug() << "Invalid row for room with UUID:" << uuid << ". Cannot remove.";
        return;
    }

    // Supprimer la salle de manière sécurisée
    beginRemoveRows(QModelIndex(), row, row);
    parentItem->removeChild(row);
    endRemoveRows();

    qDebug() << "Room with UUID:" << uuid << "successfully removed.";
}
