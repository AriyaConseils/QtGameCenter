#include "messagedelegate.h"
#include <QPainter>
#include <QApplication>
#include <QPainterPath>
#include <QTextOption>
#include <QDebug>

MessageDelegate::MessageDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void MessageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    // Enable antialiasing for smoother graphics
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);

    QString text = index.data(Qt::DisplayRole).toString();
    QString timestamp = index.data(Qt::UserRole).toString();
    QString role = index.data(Qt::UserRole + 1).toString();
    QVariantList styleParams = index.data(Qt::UserRole + 2).toList();

    QColor userBubbleColor = styleParams.value(0, QColor("#87CEEB")).value<QColor>();
    QColor botBubbleColor = styleParams.value(1, QColor(Qt::gray)).value<QColor>();
    QColor timestampColor = styleParams.value(2, QColor(Qt::lightGray)).value<QColor>();
    QColor textColor = styleParams.value(3, QColor(Qt::white)).value<QColor>();
    int textSize = styleParams.value(4, option.font.pointSize()).toInt();
    int timestampSize = styleParams.value(5, option.font.pointSize() - 2).toInt();

    QFont font = option.font;
    font.setPointSize(textSize);
    painter->setFont(font);
    QFontMetrics fm(font);

    QFont timestampFont = option.font;
    timestampFont.setPointSize(timestampSize);
    QFontMetrics timestampFm(timestampFont);

    int maxBubbleWidth = option.rect.width() - 40; // Limit the width of the bubble to the view's width minus 40

    // Calculate the text bounding rect
    QTextOption textOption;
    textOption.setWrapMode(QTextOption::WordWrap);
    QRectF textRect = painter->boundingRect(QRect(0, 0, maxBubbleWidth - 20, 0), text, textOption);
    int textBlockHeight = static_cast<int>(textRect.height());

    // Calculate the total bubble height and width
    int bubbleHeight = textBlockHeight + timestampFm.height() + 20; // 20 is for padding
    int bubbleWidth = qMin(maxBubbleWidth, static_cast<int>(textRect.width()) + 20);

    QRect bubbleRect = option.rect.adjusted(0, 0, 0, -10); // Add space between bubbles
    if (role == "user") {
        painter->setBrush(userBubbleColor);
        bubbleRect.setLeft(bubbleRect.right() - bubbleWidth - 20);
    } else {
        painter->setBrush(botBubbleColor);
        bubbleRect.setLeft(bubbleRect.left() + 20);
    }

    bubbleRect.setWidth(bubbleWidth);
    bubbleRect.setHeight(bubbleHeight);

    // Draw shadow
    QPainterPath path;
    path.addRoundedRect(bubbleRect.adjusted(2, 2, 2, 2), 10, 10);
    painter->fillPath(path, QColor(0, 0, 0, 50));

    // Draw bubble
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(bubbleRect, 10, 10);

    // Draw text
    painter->setPen(textColor);
    QRect textBubbleRect = bubbleRect.adjusted(10, 10, -10, -timestampFm.height() - 10);
    painter->drawText(textBubbleRect, text, textOption);

    // Draw timestamp
    painter->setPen(timestampColor);
    painter->setFont(timestampFont);
    QRect timestampRect;
    if (role == "user") {
        timestampRect = bubbleRect.adjusted(bubbleWidth - timestampFm.horizontalAdvance(timestamp) - 15, bubbleHeight - timestampFm.height() - 10, -10, -10);
    } else {
        timestampRect = bubbleRect.adjusted(10, bubbleHeight - timestampFm.height() - 10, -bubbleWidth + timestampFm.horizontalAdvance(timestamp) + 15, -10);
    }
    painter->drawText(timestampRect, timestamp);

    painter->restore();
}

QSize MessageDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QFont font = option.font;
    QString text = index.data(Qt::DisplayRole).toString();
    QVariantList styleParams = index.data(Qt::UserRole + 2).toList();
    int textSize = styleParams.value(4, option.font.pointSize()).toInt();
    int timestampSize = styleParams.value(5, option.font.pointSize() - 2).toInt();

    font.setPointSize(textSize);
    QFontMetrics textMetrics(font);

    font.setPointSize(timestampSize);
    QFontMetrics timestampMetrics(font);

    int maxBubbleWidth = option.rect.width() - 40;
    QTextOption textOption;
    textOption.setWrapMode(QTextOption::WordWrap);
    QRectF textRect(0, 0, maxBubbleWidth - 20, 0);
    QRectF boundedRect = textMetrics.boundingRect(textRect.toRect(), Qt::TextWordWrap, text);

    int bubbleHeight = static_cast<int>(boundedRect.height()) + timestampMetrics.height() + 20;

    return QSize(maxBubbleWidth, bubbleHeight + 10);
}
