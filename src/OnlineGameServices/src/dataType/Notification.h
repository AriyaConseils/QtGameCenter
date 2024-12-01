#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>

#include <QDebug>
#include "framework/helpers.h" // Assure que DECLARE_PROPERTY est défini ici

class Notification : public QObject
{
    Q_OBJECT

    // Déclaration des propriétés avec la macro
    DECLARE_PROPERTY(QString, referenceTime, setReferenceTime, "")
    DECLARE_PROPERTY(QString, sender, setSender, "")
    DECLARE_PROPERTY(QString, type, setType, "")
    DECLARE_PROPERTY(QString, serviceName, setServiceName, "")
    DECLARE_PROPERTY(QJsonObject, data, setData, QJsonObject())

public:
    // Constructeur par défaut
    explicit Notification(QObject *parent = nullptr) : QObject(parent) {}

    // Constructeur avec QJsonObject
    explicit Notification(const QJsonObject &json, QObject *parent = nullptr)
        : QObject(parent)
    {
        setReferenceTime(json.value("referenceTime").toString());
        setSender(json.value("sender").toString());
        setType(json.value("type").toString());

        if (json.contains("subscribeService")) {
            QJsonObject subscribeService = json.value("subscribeService").toObject();
            setServiceName(subscribeService.value("serviceName").toString());
            setData(subscribeService.value("data").toObject());
        }
    }

    // Opérateur de copie
    Notification(const Notification &other)
        : QObject(other.parent())
    {
        setReferenceTime(other.referenceTime());
        setSender(other.sender());
        setType(other.type());
        setServiceName(other.serviceName());
        setData(other.data());
    }

    Notification &operator=(const Notification &other)
    {
        if (this != &other) {
            setReferenceTime(other.referenceTime());
            setSender(other.sender());
            setType(other.type());
            setServiceName(other.serviceName());
            setData(other.data());
        }
        return *this;
    }

    // Opérateur d'égalité
    bool operator==(const QString &type) const
    {
        return serviceName() == type;
    }

    // Surcharge de QDebug
    friend QDebug operator<<(QDebug debug, const Notification &notification)
    {
        debug.nospace() << "Notification("
                        << "referenceTime: " << notification.referenceTime() << ", "
                        << "sender: " << notification.sender() << ", "
                        << "type: " << notification.type() << ", "
                        << "serviceName: " << notification.serviceName() << ", "
                        << "data: " << QJsonDocument(notification.data()).toJson(QJsonDocument::Compact)
                        << ")";
        return debug.space();
    }

signals:
    void referenceTimeChanged();
    void senderChanged();
    void typeChanged();
    void serviceNameChanged();
    void dataChanged();
};

#endif // NOTIFICATION_H
