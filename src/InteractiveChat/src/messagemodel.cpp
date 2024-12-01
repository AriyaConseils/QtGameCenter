#include "messagemodel.h"

MessageModel::MessageModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void MessageModel::addMessage(const Message &message)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_messages << message;
    endInsertRows();
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_messages.count();
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const Message &message = m_messages[index.row()];
    if (role == Qt::DisplayRole) {
        return message.text();
    } else if (role == Qt::UserRole) {
        return message.timestamp();
    } else if (role == Qt::UserRole + 1) {
        return message.role();
    } else if (role == Qt::UserRole + 2) {
        return message.styleParams();
    }

    return QVariant();
}

void MessageModel::setMessageStyle(int index, const QVariantList &styleParams)
{
    if (index < 0 || index >= m_messages.size())
        return;

    m_messages[index].setStyleParams(styleParams);
    emit dataChanged(this->index(index), this->index(index), {Qt::UserRole + 2});
}
