#include "QOnlineGameCenter.h"
#include <QSettings>



QOnlineGameCenter::QOnlineGameCenter(const QString& httpAddress, const QString& wsAddress, QObject *parent)
    : QObject(parent),
    m_wisperInterface(new QWisperInterface(httpAddress, wsAddress)),
    m_roomModel(new RoomModel(this)),
    m_playerModel(new PlayerModel(this)),
    m_proxyFriendList(new TablePlayerProxyModel()),
    m_localPlayer(new Player("Eirik", "Eng"))
{
    m_proxyFriendList->setSourceModel(m_playerModel);
    setupConnections();

    m_notificationHandlers = {
        { "UserStatus", [this](const QString& senderId, const QJsonObject& data) {
             _notifUserStatusChanged(senderId, data);
         }},
        { "UserInformation", [this](const QString& senderId, const QJsonObject& data) {
             _notifUserInformationChanged(senderId, data["UserInformation"].toObject());
         }},
        { "OpenedTable", [this](const QString& senderId, const QJsonObject& data) {
             _notifTableInformationChanged(senderId, data);
         }}
    };

    CONNECT_PROP_VALUE(Player, m_localPlayer, gamesPlayed, [this](QVariant value) {
        m_wisperInterface->postUserInformation("gamesPlayed", value);
    });
    CONNECT_PROP_VALUE(Player, m_localPlayer, gamesWon, [this](QVariant value) {
        m_wisperInterface->postUserInformation("gamesWon", value);
    });
}


TablePlayerProxyModel* QOnlineGameCenter::getProxyFriendList() const {
    return m_proxyFriendList;
}

PlayerModel* QOnlineGameCenter::getPlayerModel() const {
    return m_playerModel;
}


RoomModel *QOnlineGameCenter::roomModel() const
{
    return m_roomModel;
}


void QOnlineGameCenter::connectToServer(std::function<QJsonObject()> signUpCallback) {
    QJsonObject userInfo = getUserInfoFromSettings();
    *m_localPlayer = Player::fromJson(userInfo);
    if (!attemptConnection(userInfo)) {
        if (signUpCallback) {
            userInfo = signUpCallback();
            *m_localPlayer = Player::fromJson(userInfo);
            if (handleSignUp(userInfo)) {
                saveUserInfoToSettings(userInfo);
            }
        }
    }

    setupAuthenticationErrorHandler(signUpCallback);
}

// Récupère les informations utilisateur depuis QSettings
QJsonObject QOnlineGameCenter::getUserInfoFromSettings() {
    QSettings settings("AriyaConsulting", "TicTacToe");
    return settings.value("userInfo").toJsonValue().toObject();
}

// Enregistre les informations utilisateur dans QSettings
void QOnlineGameCenter::saveUserInfoToSettings(const QJsonObject& info) {
    QSettings settings("AriyaConsulting", "TicTacToe");
    settings.setValue("userInfo", info);
}


// Tente de se connecter au serveur avec les informations utilisateur
bool QOnlineGameCenter::attemptConnection(const QJsonObject& info) {
    QString firstName = info.value("firstName").toString();
    QString lastName = info.value("lastName").toString();
    QString password = info.value("password").toString();

    if (firstName.isEmpty() || lastName.isEmpty() || password.isEmpty()) {
        qDebug() << "Missing or incomplete information.";
        return false;
    }

    m_wisperInterface->connect(firstName, lastName, password);
    return true;
}

// Gère la tentative d'inscription
bool QOnlineGameCenter::handleSignUp(const QJsonObject& info) {
    if (info.isEmpty() || !info.contains("firstName") || !info.contains("lastName") || !info.contains("password")) {
        qDebug() << "Sign-up information is incomplete.";
        return false;
    }

    m_wisperInterface->signUp(info["firstName"].toString(), info["lastName"].toString(), info["password"].toString());
    disconnect(m_wisperInterface, &QWisperInterface::userCreated, this, nullptr);
    connect(m_wisperInterface, &QWisperInterface::userCreated, this, [this, info]() {
        QJsonObject storedInfo = info;
        storedInfo.remove("password");
        m_wisperInterface->postUserInformation(storedInfo);
        saveUserInfoToSettings(info);
    });

    qDebug() << "Sign-up attempt made with:" << info["firstName"].toString() << info["lastName"].toString();
    return true;
}

// Configure le gestionnaire d'erreur d'authentification
void QOnlineGameCenter::setupAuthenticationErrorHandler(std::function<QJsonObject()> signUpCallback) {
    disconnect(m_wisperInterface, &QWisperInterface::authentificationError, this, nullptr);
    connect(m_wisperInterface, &QWisperInterface::authentificationError, this, [this, signUpCallback]() {
        if (signUpCallback) {
            QJsonObject info = signUpCallback();
            if (!handleSignUp(info)) {
                qWarning() << "Something went wrong: impossible to create a new account with the provided information:"
                           << info;
            }
        }
    });
}


void QOnlineGameCenter::setupConnections() {
    connect(m_wisperInterface, &QWisperInterface::webSocketConnected, this, &QOnlineGameCenter::handleWebSocketConnected);
    connect(m_wisperInterface, &QWisperInterface::webSocketDisconnected, this, &QOnlineGameCenter::handleWebSocketDisconnected);
    connect(m_wisperInterface, &QWisperInterface::connectionErrorOccurred, this, &QOnlineGameCenter::serverError);

    connect(m_wisperInterface, &QWisperInterface::newNotification, this, &QOnlineGameCenter::handleNewNotification);
    connect(m_wisperInterface, &QWisperInterface::userInformationChanged, this, &QOnlineGameCenter::_notifUserInformationChanged);

    connect(m_wisperInterface, &QWisperInterface::newMessage, this, [](const QString &message) {
        //TODO connecter le chat
    });

}


void QOnlineGameCenter::handleWebSocketConnected(QWebSocket *webSocket) {
    Q_UNUSED(webSocket)
    emit serverConnected();
    m_localPlayer->setOnline(true);
    m_wisperInterface->fetchUsers();
}

void QOnlineGameCenter::handleWebSocketDisconnected() {
    emit serverDisconnected();
    m_localPlayer->setOnline(false);
}


void QOnlineGameCenter::handleNewNotification(Notification newNotif) {
    auto handler = m_notificationHandlers.value(newNotif.serviceName(), nullptr);
    if (handler) {
        handler(newNotif.sender(), newNotif.data());
    } else {
        qWarning() << "No handler found for notification:" << newNotif;
    }
}



void QOnlineGameCenter::postTableInformation(const QJsonObject &roomInfo)
{
    pushTableInfoIntoModel(roomInfo);
    QJsonObject OpenedTableJson;
    OpenedTableJson["OpenedTable"] = roomInfo;
    m_wisperInterface->postNotification(OpenedTableJson, true);
}

Player *QOnlineGameCenter::localPlayer() const
{
    return m_localPlayer;
}



void QOnlineGameCenter::_notifUserStatusChanged(const QString& senderId, const QJsonObject& data) {
    if (Player* currentPlayer = m_playerModel->getPlayer(senderId)) {
        currentPlayer->setOnline(data["connected"].toBool());
        int row = m_playerModel->getPlayerRow(senderId);
        if (row != -1) {
            QModelIndex index = m_playerModel->index(row, 0);
            emit m_playerModel->dataChanged(index, index, {PlayerModel::OnlineRole});
        }
    }
}

void QOnlineGameCenter::pushTableInfoIntoModel(const QJsonObject& roomInfo)
{
    if (!roomInfo["active"].toBool()) {
        m_roomModel->removeRoom(roomInfo["uuid"].toString());
    } else {
        m_roomModel->addOrUpdateRoom(roomInfo);
    }
}

void QOnlineGameCenter::_notifTableInformationChanged(const QString &id, const QJsonObject& roomInfo)
{
    Q_UNUSED(id)
    pushTableInfoIntoModel(roomInfo);
    emit tableInformationChanged(roomInfo);
}


void QOnlineGameCenter::_notifUserInformationChanged(const QString &id, const QJsonObject &infoJson) {

    Player *currentPlayer = (m_playerModel->getPlayer(id)) ? m_playerModel->getPlayer(id) : new Player(id);

    for (const QString &key : infoJson.keys()) {
        const QJsonValue &value = infoJson.value(key);
        QVariant variantValue = value.toVariant();

        if (!currentPlayer->setProperty(key.toUtf8().constData(), variantValue)) {
            qDebug() << "Failed to set property" << key << "with value" << variantValue;
        }
    }

    if( m_playerModel->getPlayer(id)){
        int row = m_playerModel->getPlayerRow(id);
        if (row != -1) {
            QModelIndex index = m_playerModel->index(row, 0);
            emit m_playerModel->dataChanged(index, index);
        }
    } else {

        if (id != m_wisperInterface->getSessionId()) {
            m_proxyFriendList->addPlayerId(id);
        } else {
            *m_localPlayer = *currentPlayer;
            delete currentPlayer;
            currentPlayer = m_localPlayer;
        }

        m_playerModel->addPlayer(currentPlayer);
        emit newPlayerAdded(currentPlayer);
    }
}
