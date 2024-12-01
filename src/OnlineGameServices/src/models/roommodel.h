#ifndef ROOMMODEL_H
#define ROOMMODEL_H

#include <QAbstractItemModel>
#include "TreeItem.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>

class RoomModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit RoomModel(QObject *parent = nullptr);
    ~RoomModel();

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void setupModelData(const QJsonArray &rooms);

    void addRoom(const QJsonObject &roomInfo);
    void updateRoom(const QString &uuid, const QJsonObject &roomInfo);
    void removeRoom(const QString &uuid);
    void addOrUpdateRoom(const QJsonObject &roomInfo);

private:
    TreeItem *rootItem;
    TreeItem *findRoomByUUID(const QString &uuid) const;
};

#endif // ROOMMODEL_H
