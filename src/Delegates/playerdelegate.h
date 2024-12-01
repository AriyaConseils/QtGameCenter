#ifndef PLAYERDELEGATE_H
#define PLAYERDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

class PlayerDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    explicit PlayerDelegate(QObject* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
private:
    int m_spacing;

};

#endif // PLAYERDELEGATE_H
