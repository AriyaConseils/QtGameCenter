#include "PlayerDelegate.h"
#include <QPainter>
#include <QPainterPath>

PlayerDelegate::PlayerDelegate(QObject* parent)
    : QStyledItemDelegate(parent) {}

void PlayerDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();

    // Adjust the item rectangle to add padding
    const int padding = 10;
    QRectF adjustedRect = option.rect.adjusted(padding, padding / 2, -padding, -padding / 2);

    // Draw the rounded background for the item
    QPainterPath backgroundPath;
    backgroundPath.addRoundedRect(adjustedRect, 10, 10); // Rounded corners with a radius of 10
    QColor backgroundColor = Qt::white; // Default background color
    if (option.state & QStyle::State_Selected) {
        backgroundColor = QColor(220, 240, 255); // Light blue for selection
    } else if (option.state & QStyle::State_MouseOver) {
        backgroundColor = QColor(240, 248, 255); // Slightly blue for hover
    }
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillPath(backgroundPath, backgroundColor);

    // Draw the border around the item
    painter->setPen(QPen(QColor(200, 200, 200), 1));
    painter->drawPath(backgroundPath);

    // Retrieve model data: first name, last name, image, and connection status
    QString firstName = index.data(Qt::UserRole + 1).toString();
    QString lastName = index.data(Qt::UserRole + 2).toString();
    QPixmap image = qvariant_cast<QPixmap>(index.data(Qt::UserRole + 3));
    bool isOnline = index.data(Qt::UserRole + 4).toBool();

    // Draw the rounded player image or placeholder
    const int imageSize = adjustedRect.height() - 20; // Define the image size
    QRectF imageRect(adjustedRect.left() + 10, adjustedRect.top() + 10, imageSize, imageSize);
    if (!image.isNull()) {
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        QPixmap scaledImage = image.scaled(imageRect.size().toSize(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        QPainterPath imagePath;
        imagePath.addEllipse(imageRect); // Circular mask
        painter->setClipPath(imagePath);
        painter->drawPixmap(imageRect.toRect(), scaledImage);
        painter->setClipping(false);
    } else {
        painter->setBrush(Qt::gray);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(imageRect); // Placeholder as a gray circle
    }

    // Draw the status dot (green or red) with a white border
    const int circleRadius = 8;
    QRectF circleRect(imageRect.right() - circleRadius - 5, imageRect.bottom() - circleRadius - 5, 2 * circleRadius, 2 * circleRadius);
    painter->setBrush(Qt::white); // White border
    painter->drawEllipse(circleRect.adjusted(-2, -2, 2, 2));
    painter->setBrush(isOnline ? Qt::green : Qt::red); // Green for online, red for offline
    painter->drawEllipse(circleRect);

    // Draw the player's name (first and last name)
    QRectF textRect(imageRect.right() + 10, adjustedRect.top(), adjustedRect.width() - imageRect.width() - 20, adjustedRect.height());
    QString fullName = firstName + " " + lastName;
    painter->setPen(QPen(Qt::black));
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, fullName);

    painter->restore();
}

QSize PlayerDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    Q_UNUSED(option);
    Q_UNUSED(index);

    // Define the item size including padding
    return QSize(220, 70);
}
