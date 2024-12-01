#ifndef QWISPERINTERFACE_H
#define QWISPERINTERFACE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QUrlQuery>
#include <QRandomGenerator>
#include <QTimer>

#include <QNetworkInterface>
#include <QList>
#include <QDebug>
#include <QCryptographicHash>
#include "dataType/Notification.h"


class QWisperInterface : public QObject
{
    Q_OBJECT
public:
    explicit QWisperInterface(const QString &serverUrl, const QString &webSocketUrl, QObject *parent = nullptr);
    ~QWisperInterface();
    void connect(const QString &userName, const QString &userLastName, const QString &passwordId);
    void connectWebSocket();
    void signUp(const QString &userName,const QString &userLastName, const QString &password);
    void verifyPhoneNumber(const QString &phoneNumber, const QString &otp);
    void createUser(const QString &numberPhone, const QString &AuthorizationToSignup, const QString &familyName, const QString &userName, const QString &passwordId);
    void postNotification(const QJsonObject &notification, bool unique);
    void sendMessage(const QString &contactId, const QJsonObject &message);


public slots:
    void fetchUsers();
    void getUserInformations(QStringList userList);
    void subscribeServices(const QString &user, const QStringList &servicesList, int serviceDuration = 350);
    void postUserInformation(const QJsonObject& userInfo);
    void postUserInformation(const QString& key, const QVariant &value);
    QString getSessionId() const;

signals:
    void userCreated();
    void authentificationError(const QString &);
    void webSocketConnected(QWebSocket *);
    void connectionErrorOccurred();
    void webSocketDisconnected();
    void newMessage(const QString &);
    void newNotification(Notification);
    void userInformationChanged(const QString &, const QJsonObject &);


private slots:
    void onWebSocketConnected();
    void onTextMessageReceived(const QString &message);


private:
    QNetworkRequest createRequest(const QString& route, const QMap<QString, QString>& customHeaders = QMap<QString, QString>()) const;

    inline QByteArray toByteArray(const QJsonObject& jsonObject) {
        return QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);
    }


    QNetworkAccessManager *networkManager;
    QWebSocket *webSocket = nullptr;
    QString serverUrl;
    QString webSocketUrl;
    QString symmetricDates();
    bool clientResponded;

    QString token;
    QString sessionId;
    QString name_;
    QString lastName_;
    QString m_password;
    QString m_userId;
    QString m_Otp;
};

#endif // QWISPERINTERFACE_H
