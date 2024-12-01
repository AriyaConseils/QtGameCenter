#ifndef QONLINEGAMECENTER_H
#define QONLINEGAMECENTER_H

#include <QObject>
#include <QJsonObject>
#include <QWebSocket>
#include <functional>
#include <QJsonObject>
#include "QWisperInterface.h"

#include "models/playermodel.h"
#include "models/tableplayerproxymodel.h"
#include "models/roommodel.h"

#include "framework/helpers.h"


class QOnlineGameCenter : public QObject {
    Q_OBJECT

public:
    explicit QOnlineGameCenter(const QString& httpAddress, const QString& wsAddress, QObject *parent = nullptr);

    TablePlayerProxyModel* getProxyFriendList() const;
    PlayerModel* getPlayerModel() const;
    void connectToServer(std::function<QJsonObject()> signUpCallback);

    RoomModel *roomModel() const;

    Player *localPlayer() const;

public slots:
    void postTableInformation(const QJsonObject &roomInfo);

private:
    QWisperInterface *m_wisperInterface;
    RoomModel *m_roomModel;
    PlayerModel *m_playerModel;
    TablePlayerProxyModel *m_proxyFriendList;
    Player *m_localPlayer;
    QMap<QString, std::function<void(const QString&, const QJsonObject&)>> m_notificationHandlers;

    void setupConnections();
    void pushTableInfoIntoModel(const QJsonObject& roomInfo);

signals:
    void serverConnected();
    void serverDisconnected();
    void serverError();
    void newPlayerAdded(Player *player);
    void postUserInformation(QString property, QVariant information);
    void tableInformationChanged(const QJsonObject &);

private slots:
    void handleWebSocketConnected(QWebSocket *webSocket);
    void handleWebSocketDisconnected();

    void _notifUserInformationChanged(const QString &id, const QJsonObject &infoJson);
    void _notifTableInformationChanged(const QString &id, const QJsonObject& roomInfo);
    void _notifUserStatusChanged(const QString& senderId, const QJsonObject& data);

    void handleNewNotification(Notification newNotif);
    void setupAuthenticationErrorHandler(std::function<QJsonObject()> signUpCallback);
    // Gère la tentative d'inscription
    bool handleSignUp(const QJsonObject& info);
    // Tente de se connecter au serveur avec les informations utilisateur
    bool attemptConnection(const QJsonObject& info);
    // Enregistre les informations utilisateur dans QSettings
    void saveUserInfoToSettings(const QJsonObject& info);
    // Récupère les informations utilisateur depuis QSettings
    QJsonObject getUserInfoFromSettings();
};

#endif // QONLINEGAMECENTER_H
