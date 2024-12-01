#include <QElapsedTimer>
#include <QDebug>


#define DECLARE_PROPERTY(type, propName, setter, initVal) \
Q_PROPERTY(type propName READ propName WRITE setter NOTIFY propName##Changed) \
    public: \
    type propName() const { return m_##propName; } \
    void setter(const type &value) { \
        if (m_##propName != value) { \
            m_##propName = value; \
            emit propName##Changed(); \
    } \
} \
    private: \
    type m_##propName = initVal;

#define DECLARE_PROPERTY_CB(type, propName, setter, initVal) \
Q_PROPERTY(type propName READ propName WRITE setter NOTIFY propName##Changed) \
    public: \
    type propName() const { return m_##propName; } \
    void setter(const type &value, bool forceCustomAction = true) { \
        if (forceCustomAction || m_##propName != value) { \
            if (m_##propName != value) { \
                m_##propName = value; \
                emit propName##Changed(); \
        } \
            on##propName##ChangedAction(value); /* Appel de l'action personnalisée */ \
    } \
} \
    private: \
    type m_##propName = initVal; \
    protected: \
    virtual void on##propName##ChangedAction(const type &_##propName)



#define CONNECT_PROP(type, sender, prop_sent, receiver, prop_received) \
do { \
        QObject *capturedSender = sender; \
        QObject *capturedReceiver = receiver; \
        const QMetaObject *metaObjectSender = capturedSender->metaObject(); \
        const QMetaObject *metaObjectReceiver = capturedReceiver->metaObject(); \
    \
        int senderPropertyIndex = metaObjectSender->indexOfProperty(#prop_sent); \
        int receiverPropertyIndex = metaObjectReceiver->indexOfProperty(#prop_received); \
    \
        /* Vérification des propriétés avec un message dans Q_ASSERT */ \
        Q_ASSERT_X(senderPropertyIndex != -1, "CONNECT_PROP_TRANFORM", \
                                                                       QString("Sender property: %1 does not exist!").arg(#prop_sent).toUtf8().constData()); \
        Q_ASSERT_X(receiverPropertyIndex != -1, "CONNECT_PROP_TRANFORM", \
                                                                         QString("Receiver property: %1 does not exist!").arg(#prop_received).toUtf8().constData()); \
    \
        /* Initialisation de la propriété cible avec la valeur actuelle de la source */ \
        QVariant initialValue = capturedSender->property(#prop_sent); \
        capturedReceiver->setProperty(#prop_received, initialValue); \
    \
        /* Connexion dynamique */ \
        connect(sender, &type::prop_sent##Changed, receiver, [capturedSender, capturedReceiver]() { \
                QVariant value = capturedSender->property(#prop_sent); \
                capturedReceiver->setProperty(#prop_received, value); \
        }); \
} while (0)


#define CONNECT_PROP_SLOT(type, sender, prop_sent, receiver, slot_received) \
    do { \
        QObject *capturedSender = sender; \
        QObject *capturedReceiver = receiver; \
        const QMetaObject *metaObjectSender = capturedSender->metaObject(); \
        const QMetaObject *metaObjectReceiver = capturedReceiver->metaObject(); \
    \
        int senderPropertyIndex = metaObjectSender->indexOfProperty(#prop_sent); \
        int slotIndex = metaObjectReceiver->indexOfSlot(QMetaObject::normalizedSignature(#slot_received "(QVariant)").constData()); \
    \
        /* Vérification des propriétés et du slot avec un message dans Q_ASSERT */ \
        Q_ASSERT_X(senderPropertyIndex != -1, "CONNECT_PROP_SLOT", \
                                                                   QString("Sender property: %1 does not exist!").arg(#prop_sent).toUtf8().constData()); \
        Q_ASSERT_X(slotIndex != -1, "CONNECT_PROP_SLOT", \
                                                         QString("Receiver slot: %1 does not exist or has invalid signature!").arg(#slot_received).toUtf8().constData()); \
    \
        /* Initialisation du slot avec la valeur actuelle de la propriété source */ \
        QVariant initialValue = capturedSender->property(#prop_sent); \
        QMetaObject::invokeMethod(capturedReceiver, #slot_received, Qt::DirectConnection, Q_ARG(QVariant, initialValue)); \
    \
        /* Connexion dynamique */ \
        connect(sender, &type::prop_sent##Changed, receiver, [capturedSender, capturedReceiver]() { \
                QVariant value = capturedSender->property(#prop_sent); \
                QMetaObject::invokeMethod(capturedReceiver, #slot_received, Qt::QueuedConnection, Q_ARG(QVariant, value)); \
        }); \
} while (0)





#define CONNECT_PROP_TRANSFORM(type, sender, prop_sent, receiver, prop_received, transform) \
    do { \
            QObject *capturedSender = sender; \
            QObject *capturedReceiver = receiver; \
            const QMetaObject *metaObjectSender = capturedSender->metaObject(); \
            const QMetaObject *metaObjectReceiver = capturedReceiver->metaObject(); \
    \
            int senderPropertyIndex = metaObjectSender->indexOfProperty(#prop_sent); \
            int receiverPropertyIndex = metaObjectReceiver->indexOfProperty(#prop_received); \
    \
            /* Vérification des propriétés avec un message dans Q_ASSERT */ \
            Q_ASSERT_X(senderPropertyIndex != -1, "CONNECT_PROP_TRANFORM", \
                                                                           QString("Sender property: %1 does not exist!").arg(#prop_sent).toUtf8().constData()); \
            Q_ASSERT_X(receiverPropertyIndex != -1, "CONNECT_PROP_TRANFORM", \
                                                                             QString("Receiver property: %1 does not exist!").arg(#prop_received).toUtf8().constData()); \
    \
            /* Initialisation de la propriété cible avec la valeur actuelle de la source */ \
            QVariant initialValue = transform(capturedSender->property(#prop_sent)); \
            capturedReceiver->setProperty(#prop_received, initialValue); \
    \
            /* Connexion dynamique */ \
            connect(sender, &type::prop_sent##Changed, receiver, [this, capturedSender, capturedReceiver]() { \
                    QVariant value = transform(capturedSender->property(#prop_sent)); \
                    capturedReceiver->setProperty(#prop_received, value); \
            }); \
} while (0)



#define CONNECT_PROP_VALUE(type, sender, prop_sent, transform) \
do { \
        QObject *capturedSender = sender; \
        const QMetaObject *metaObjectSender = capturedSender->metaObject(); \
\
        int senderPropertyIndex = metaObjectSender->indexOfProperty(#prop_sent); \
\
        /* Vérification de l'existence de la propriété avec un message dans Q_ASSERT */ \
        Q_ASSERT_X(senderPropertyIndex != -1, "CONNECT_PROP_VALUE", \
                                                                    QString("Sender property: %1 does not exist!").arg(#prop_sent).toUtf8().constData()); \
\
        /* Initialisation avec la valeur actuelle de la propriété source */ \
        transform(capturedSender->property(#prop_sent)); \
\
        /* Connexion dynamique pour capturer les changements de la propriété */ \
        connect(sender, &type::prop_sent##Changed, [this, capturedSender]() { \
                QVariant value = capturedSender->property(#prop_sent); \
                transform(value); \
        }); \
} while (0)


#define MONITOR_FUNCTION_TIME \
QElapsedTimer timer;      \
    timer.start();            \
    auto onExit = qScopeGuard([&]() { \
              qDebug() << "Function" << __FUNCTION__ << "executed in" << timer.elapsed() << "ms"; \
      });
