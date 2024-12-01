#include "QWisperInterface.h"

#include <QByteArray>
#include <QDataStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>




QWisperInterface::QWisperInterface(const QString &serverUrl, const QString &webSocketUrl, QObject *parent)
    : QObject(parent), serverUrl(serverUrl), webSocketUrl(webSocketUrl), clientResponded(false), m_Otp("123456")
{
    networkManager = new QNetworkAccessManager(this);

}

QWisperInterface::~QWisperInterface()
{
    delete networkManager;
    delete webSocket;
    networkManager = nullptr;
    webSocket = nullptr;

    qDebug() << "ServerManager destroyed";
}

QNetworkRequest QWisperInterface::createRequest(const QString& route, const QMap<QString, QString>& customHeaders) const {
    QUrl url(serverUrl + route);
    QNetworkRequest request(url);

    // En-têtes par défaut
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("sessionid", sessionId.toUtf8());
    request.setRawHeader("authorization", token.toUtf8());

    // Ajout des en-têtes personnalisés
    for (auto it = customHeaders.begin(); it != customHeaders.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }

    return request;
}


void QWisperInterface::connect(const QString &userName, const QString &userLastName, const QString &passwordId)
{
    m_userId = userName+userLastName;
    m_password = passwordId;
    name_ = userName;
    lastName_ = userLastName;

    QJsonObject json;
    json["ident"] = m_userId;
    json["password"] = passwordId;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkRequest request = createRequest(QStringLiteral("/Auth"));
    QNetworkReply *reply = networkManager->post(request, data);

    QObject::connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError)
        {
            QJsonDocument responseDoc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject responseObject = responseDoc.object();
            token = responseObject["Authorization"].toString();
            sessionId = responseObject["sessionId"].toString();
            connectWebSocket();
        }
        else
        {
            qWarning() << "Authentication failed: " << reply->errorString();
            emit authentificationError(reply->errorString());
        }
        reply->deleteLater();
    });
}


void QWisperInterface::signUp(const QString &userName,const QString &userLastName, const QString &password)
{
    m_userId = userName+userLastName;
    m_password = password;
    name_ = userName;
    lastName_ = userLastName;

    QJsonObject json;
    json["numberPhone"] = m_userId;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkRequest request = createRequest(QStringLiteral("/signup"));
    QNetworkReply *reply = networkManager->post(request, data);

    QObject::connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError)
        {
            QJsonDocument responseDoc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject responseObject = responseDoc.object();
            QString numberPhone = responseObject["numberPhone"].toString();

            this->verifyPhoneNumber(numberPhone, m_Otp);
        }
        else
        {
            qWarning() << "Sign up failed: " << reply->errorString();
        }
        reply->deleteLater();
    });
}

void QWisperInterface::verifyPhoneNumber(const QString &phoneNumber, const QString &otp)
{
    QJsonObject json;
    json["numberPhone"] = phoneNumber;
    json["verification"] = otp;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkRequest request = createRequest(QStringLiteral("/verify"));
    QNetworkReply *reply = networkManager->post(request, data);

    QObject::connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError)
        {
            QJsonDocument responseDoc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject responseObject = responseDoc.object();
            QString AuthorizationToSignup = responseObject["AuthorizationToSignup"].toString();
            QString numberPhone = responseObject["numberPhone"].toString();

            this->createUser(numberPhone, AuthorizationToSignup, name_, lastName_, m_password);
        }
        else
        {
            qWarning() << "Verification failed: " << reply->errorString();
        }
        reply->deleteLater();
    });
}

void QWisperInterface::createUser(const QString &numberPhone, const QString &AuthorizationToSignup, const QString &familyName, const QString &userName, const QString &passwordId)
{
    QJsonObject json;
    json["numberPhone"] = numberPhone;
    json["AuthorizationToSignup"] = AuthorizationToSignup;
    json["familyName"] = familyName;
    json["userName"] = userName;
    json["password"] = passwordId;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkRequest request = createRequest(QStringLiteral("/createuser"));
    QNetworkReply *reply = networkManager->post(request, data);

    QObject::connect(reply, &QNetworkReply::finished, this, [this, reply, numberPhone]() {
        if (reply->error() == QNetworkReply::NoError)
        {
            QJsonDocument responseDoc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject responseObject = responseDoc.object();
            token = responseObject["Authorization"].toString();
            sessionId = numberPhone;

            connectWebSocket();
            emit userCreated();
        }  else {
           qWarning() << "Create user failed: " << reply->errorString();
        }
        reply->deleteLater();
    });
}


void QWisperInterface::connectWebSocket()
{
    QJsonObject json;
    json["seedKey"] =  symmetricDates();

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkRequest request = createRequest(QStringLiteral("/GrantChatAccess"));
    QNetworkReply *reply = networkManager->post(request, data);

    QObject::connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError)
        {
            QJsonDocument responseDoc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject responseObject = responseDoc.object();
            QString serverKey = responseObject["key"].toString();

            QUrl wsUrl(webSocketUrl);
            QUrlQuery query;
            query.addQueryItem("cryptedKey", serverKey);
            wsUrl.setQuery(query);

            if(webSocket){
                delete webSocket;
            }
            webSocket = new QWebSocket();

            QObject::connect(webSocket, &QWebSocket::connected, this, &QWisperInterface::onWebSocketConnected);

            QObject::connect(webSocket, QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors), this, [this](const QList<QSslError> &errors) {
                qWarning() << "WebSocket SSL errors occurred:";
                for (const QSslError &error : errors) {
                    qWarning() << error.errorString();
                }
            });

            QObject::connect(webSocket, &QWebSocket::errorOccurred, this, [this](QAbstractSocket::SocketError error) {
               qWarning() << "WebSocket error occurred:" << error;
                emit connectionErrorOccurred();
            });

            QObject::connect(webSocket, &QWebSocket::disconnected, this, [this]() {
                qWarning() << "WebSocket disconnected, attempting to reconnect in 1 second...";
                emit webSocketDisconnected();

                QTimer::singleShot(3000, this, [this]() {
                    connectWebSocket();
                });
            });

            QObject::connect(webSocket, &QWebSocket::textMessageReceived, this, &QWisperInterface::onTextMessageReceived);
            webSocket->open(wsUrl);
        }
        else
        {
            qWarning() << "Grant chat access failed: " << reply->errorString();
            connectWebSocket();
        }
        reply->deleteLater();
    });

    QObject::connect(reply, &QNetworkReply::errorOccurred, this, [this](QNetworkReply::NetworkError error) {
        qWarning() << "Network error occurred:" << error;
    });

    QObject::connect(reply, &QNetworkReply::sslErrors, this, [this](const QList<QSslError> &errors) {
        qWarning() << "Network SSL errors occurred:";
        for (const QSslError &error : errors) {
            qWarning() << error.errorString();
        }
    });
}



void QWisperInterface::onWebSocketConnected()
{
    emit webSocketConnected(webSocket);
}

void QWisperInterface::onTextMessageReceived(const QString &message)
{
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject messageObject = doc.object();

    QString messageType = messageObject.value("type").toString();

    if (messageType == "keepAlive")
    {
        clientResponded = true;
        messageObject["type"] = "keepAliveResponse";

        QJsonDocument doc(messageObject);
        QString messageString = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
        if(webSocket && webSocket->isValid()){
            webSocket->sendTextMessage(messageString);
        }
    }
    else
    {
        QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());
        QJsonObject messsageObject = jsonDoc.object();

        if(messsageObject.contains("type") && messsageObject.value("type").toString() == "Notification" && messsageObject.contains("data")){
            foreach(const QString &key, messsageObject["data"].toObject().keys()){
                Notification newNotif(messsageObject["data"][key].toObject());
                emit newNotification(newNotif);
            }
        }
        emit newMessage(message);
    }
}




QString QWisperInterface::symmetricDates()
{
    QDate currentDate = QDate::currentDate();
    int randomDays = QRandomGenerator::global()->bounded(1, 101);

    QDate futureDate = currentDate.addDays(randomDays);
    QDate pastDate = currentDate.addDays(-randomDays);

    QString formattedFutureDate = QString::number(futureDate.year() * 12 + futureDate.month()).rightJustified(4, '0') + QString::number(futureDate.day()).rightJustified(2, '0');
    QString formattedPastDate = QString::number(pastDate.year() * 12 + pastDate.month()).rightJustified(4, '0') + QString::number(pastDate.day()).rightJustified(2, '0');

    return formattedFutureDate + formattedPastDate;
}

QString QWisperInterface::getSessionId() const
{
    return sessionId;
}


void QWisperInterface::postNotification(const QJsonObject &notification, bool unique)
{
    QNetworkRequest request = createRequest(QStringLiteral("/publishServices"));
    if(unique) request.setRawHeader("uniquenotification", "true");


    // Envoyer la requête POST
    QNetworkReply *reply = networkManager->post(request, toByteArray(notification));

    // Connecter les signaux pour gérer la réponse et les erreurs
    QObject::connect(reply, &QNetworkReply::finished, this, [reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Failed to send notification:" << reply->errorString();
        }
        reply->deleteLater();
    });
}


void QWisperInterface::fetchUsers()
{
    QNetworkRequest request = createRequest(QStringLiteral("/api/users"));

    // Envoyer la requête GET
    QNetworkReply *reply = networkManager->get(request);

    // Connecter les signaux pour gérer la réponse et les erreurs
    QObject::connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            // Lire la réponse JSON
            QByteArray response = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

            if (jsonDoc.isArray()) {
                QJsonArray jsonArray = jsonDoc.array();
                QStringList playerList;
                std::transform(jsonArray.begin(), jsonArray.end(), std::back_inserter(playerList),
                               [](const QJsonValue &value) { return value.toString(); });

                getUserInformations(playerList);
            } else {
                qWarning() << "Invalid JSON response: not an array";
            }
        } else {
            qWarning() << "Failed to send notification:" << reply->errorString();
        }

        reply->deleteLater();
    });
}



void QWisperInterface::getUserInformations(QStringList userList)
{
    QNetworkRequest request = createRequest(QStringLiteral("/userInformation"),
                                            {{"reference", "UserInformation"},
                                             {"contactid", userList.join("|").toUtf8()}});



    // Envoyer la requête GET
    QNetworkReply *reply = networkManager->get(request);

    // Connecter les signaux pour gérer la réponse et les erreurs
    QObject::connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {

            QByteArray response = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(response);

            if (jsonDoc.isObject()) {
                QJsonObject jsonObject = jsonDoc.object();
                for (auto key : jsonObject.keys()) {
                    QJsonObject userInfo = jsonObject.value(key).toObject();

                    if(userInfo.contains("UserInformation")){
                        subscribeServices(key,
                                          {"UserStatus", "OpenedTable", "isPlaying", "UserInformation"});
                        emit userInformationChanged(key, userInfo.value("UserInformation").toObject());
                    }
                }
            } else {
                qWarning() << "Invalid JSON response: not an object";
            }
        } else {
            qWarning() << "getUserInformations-->Failed to retrieve user information:" << reply->errorString();
        }

        reply->deleteLater();
    });
}



void QWisperInterface::subscribeServices(const QString &user, const QStringList &servicesList, int serviceDuration)
{   
    QNetworkRequest request = createRequest(QStringLiteral("/subscribeServices"),
                                            {{"contactid", user.toUtf8()}});

    QJsonArray followMeOnly;
    followMeOnly.append(sessionId);

    QJsonObject serviceData;
    serviceData["durationTime"] = serviceDuration;
    serviceData["persistance"] = true;
    serviceData["contactId"] = followMeOnly;

    QJsonObject servicesJson;
    foreach (const QString key, servicesList) {
        servicesJson[key] = serviceData;
    }

    // Envoyer la requête GET
    QNetworkReply *reply = networkManager->post(request, toByteArray(servicesJson));

    // Connecter les signaux pour gérer la réponse et les erreurs
    QObject::connect(reply, &QNetworkReply::finished, this, [reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "subscribeServices-->Failed to retrieve user information:" << reply->errorString();
        }

        reply->deleteLater();
    });
}


void QWisperInterface::postUserInformation(const QString& key, const QVariant &value)
{
    // Créer un QJsonObject pour contenir les données
    QJsonObject userInfo;
    userInfo.insert(key, QJsonValue::fromVariant(value));
    postUserInformation(userInfo);
}


void QWisperInterface::postUserInformation(const QJsonObject& userInfo)
{
    QNetworkRequest request = createRequest(QStringLiteral("/userInformation"));

    QJsonObject jsonUserInfo;
    jsonUserInfo["UserInformation"] = userInfo;


    QNetworkReply *reply = networkManager->post(request, toByteArray(jsonUserInfo));

    // Connecter les signaux pour gérer la réponse et les erreurs
    QObject::connect(reply, &QNetworkReply::finished, this, [reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            qWarning() << "postUserInformation-->Failed to retrieve user information:" << reply->errorString();
        }
        reply->deleteLater();
    });
}


void QWisperInterface::sendMessage(const QString &contactId, const QJsonObject &message)
{
    QNetworkRequest request = createRequest(QStringLiteral("/message"),
                                            {{"contactid", contactId.toUtf8()}});


    QNetworkReply *reply = networkManager->post(request, toByteArray(message));

    QObject::connect(reply, &QNetworkReply::finished, this, [reply]() {
        if (reply->error() != QNetworkReply::NoError) {
           qWarning() << "Failed to send message:" << reply->errorString();
        }
        reply->deleteLater();
    });
}



