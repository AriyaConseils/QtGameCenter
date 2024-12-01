#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include <QAbstractListModel>
#include "message.h"

class MessageModel : public QAbstractListModel
{
    Q_OBJECT

public:
    MessageModel(QObject *parent = nullptr);

    void addMessage(const Message &message);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void setMessageStyle(int index, const QVariantList &styleParams);

private:
    QList<Message> m_messages;
};

#endif // MESSAGEMODEL_H
