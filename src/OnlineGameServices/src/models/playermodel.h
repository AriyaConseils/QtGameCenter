#ifndef PLAYERMODEL_H
#define PLAYERMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "games/AbstractGame/Player.h"

class PlayerModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        FirstNameRole = Qt::UserRole + 1,
        LastNameRole,
        ImageRole,
        OnlineRole,
        PlayerIdRole
    };

    explicit PlayerModel(QObject* parent = nullptr);
    ~PlayerModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int getPlayerRow(const QString& playerId) const;
    Player* getPlayer(const QString& playerId);
    void addPlayer(Player *player);

private slots:
    void onImageDownloaded(QNetworkReply* reply);

private:
    QVector<Player *> players_;
    QHash<QUrl, QPixmap> imageCache_;
    mutable QNetworkAccessManager networkManager_;
};

#endif // PLAYERMODEL_H
