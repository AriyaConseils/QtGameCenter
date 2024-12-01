#include "PlayerModel.h"

PlayerModel::PlayerModel(QObject* parent)
    : QAbstractListModel(parent) {}

PlayerModel::~PlayerModel() {}

int PlayerModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return players_.size();
}

QVariant PlayerModel::data(const QModelIndex& index, int role)  const {
    if (!index.isValid() || index.row() >= players_.size())
        return QVariant();

    Player *player = players_.at(index.row());

    switch (role) {
    case FirstNameRole:
        return player->firstName();
    case LastNameRole:
        return player->lastName();
    case ImageRole: {
        if(imageCache_.contains(player->image())) {
            return imageCache_.value(player->image());
        } else if (!player->image().startsWith("http")) {
            return QPixmap(player->image());
        } else  {
            // Télécharger l'image si elle n'est pas en cache
            QVariant url = player->image();
            QUrl u = url.toUrl();
            QNetworkRequest request(u);
            QNetworkReply* reply = networkManager_.get(request);
            connect(reply, &QNetworkReply::finished, this, [this, reply](){
                const_cast<PlayerModel*>(this)->onImageDownloaded(reply);
            });
            return QVariant();
        }
    }
    case OnlineRole:
        return player->online();
    case PlayerIdRole:
        return player->playerId();
    case Qt::ToolTipRole:
        return player->getStats();
    case Qt::DisplayRole:
        return player->firstName() + " " + player->lastName();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> PlayerModel::roleNames() const {
    return {
        { FirstNameRole, "firstName" },
        { LastNameRole, "lastName" },
        { ImageRole, "image" },
        { OnlineRole, "Online" },
        { PlayerIdRole, "Online" }
    };
}

Player* PlayerModel::getPlayer(const QString& playerId) {
    for (Player *player : players_) {
        if (player->playerId() == playerId) { // Vérifie si l'id correspond
            return player;
        }
    }
    return nullptr; // Retourne nullptr si aucun joueur ne correspond
}

int PlayerModel::getPlayerRow(const QString& playerId) const {
    for (int i = 0; i < players_.size(); ++i) {
        if (players_[i]->playerId() == playerId) {
            return i;
        }
    }
    return -1;
}

void PlayerModel::addPlayer(Player *player) {
    beginInsertRows(QModelIndex(), players_.size(), players_.size());
    players_.append(player);
    endInsertRows();
}

void PlayerModel::onImageDownloaded(QNetworkReply* reply) {
    // Vérifier si une erreur s'est produite
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error downloading image from:" << reply->url()
                   << "\nError:" << reply->errorString();

        // Placeholder pour indiquer une erreur
        QPixmap errorPixmap(50, 50); // Taille par défaut pour l'image d'erreur
        errorPixmap.fill(Qt::red);   // Fond rouge pour signaler l'erreur
        imageCache_[reply->url()] = errorPixmap;

        // Trouver l'index du joueur correspondant pour notifier la vue
        for (int i = 0; i < players_.size(); ++i) {
            if (players_[i]->image() == reply->url().toString()) {
                QModelIndex index = createIndex(i, 0);
                emit dataChanged(index, index, { ImageRole });
                break;
            }
        }

        reply->deleteLater();
        return;
    }

    // Si le téléchargement est réussi, charger les données de l'image
    QUrl imageUrl = reply->url();
    QPixmap pixmap;
    pixmap.loadFromData(reply->readAll());

    if (!pixmap.isNull()) {
        imageCache_[imageUrl] = pixmap;

        // Trouver l'index du joueur correspondant pour notifier la vue
        for (int i = 0; i < players_.size(); ++i) {
            if (players_[i]->image() == imageUrl.toString()) {
                QModelIndex index = createIndex(i, 0);
                emit dataChanged(index, index, { ImageRole });
                break;
            }
        }
    } else {
        qWarning() << "Failed to load pixmap from downloaded data for URL:" << imageUrl;

        // Placeholder pour données invalides
        QPixmap invalidPixmap(50, 50);
        invalidPixmap.fill(Qt::yellow); // Fond jaune pour indiquer des données invalides
        imageCache_[imageUrl] = invalidPixmap;

        // Notifier la vue
        for (int i = 0; i < players_.size(); ++i) {
            if (players_[i]->image() == imageUrl.toString()) {
                QModelIndex index = createIndex(i, 0);
                emit dataChanged(index, index, { ImageRole });
                break;
            }
        }
    }

    reply->deleteLater();
}
