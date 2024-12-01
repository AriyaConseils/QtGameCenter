#ifndef MESSAGEDELEGATE_H
#define MESSAGEDELEGATE_H

#include <QStyledItemDelegate>

class MessageDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    MessageDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // MESSAGEDELEGATE_H
