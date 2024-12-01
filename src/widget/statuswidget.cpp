#include "StatusWidget.h"
#include <QResizeEvent>
#include <QDebug>

StatusWidget::StatusWidget(QWidget *parent)
    : QWidget(parent)
    , m_statusIndicator(new QLabel(this))
{
    // Initializes the status widget and its indicator.
    // Sets attributes to ignore mouse events and enable styled backgrounds.
    // Calls updateIndicator() to configure the initial state.

    m_statusIndicator->setAttribute(Qt::WA_TransparentForMouseEvents); // Ignore mouse clicks
    m_statusIndicator->setAttribute(Qt::WA_StyledBackground, true);    // Enable styled backgrounds

    updateIndicator(); // Initialize the indicator
}


StatusWidget::~StatusWidget()
{
    delete m_statusIndicator;
}


void StatusWidget::resizeEvent(QResizeEvent *event)
{
    // Handles widget resizing events.
    // Updates the status indicator's size and position,
    // and ensures it remains on top after the resize.

    QWidget::resizeEvent(event);
    updateIndicator();
    ensureIndicatorOnTop();
}



void StatusWidget::updateIndicator()
{
    // Updates the size, position, and color of the status indicator.
    // The indicator size is proportional to the widget dimensions.
    // It positions the indicator in the bottom-right corner.

    const int border = 7;
    if (!m_statusIndicator) return;

    // Calculate the size of the indicator based on widget dimensions
    int size = qMin(width(), height()) / 2.8;

    m_statusIndicator->setFixedSize(size, size + 10);
    QString styleSheet = QString(
                             "border-radius: %1px;"
                             "background-color: %2;"
                             "border: 2px solid white;"
                             ).arg(size / 2)
                             .arg(m_online ? "green" : "red");

    m_statusIndicator->setStyleSheet(styleSheet);
    // Calculate the exact position for the indicator
    int x = width() - size;
    int y = height() - (size + 10);

    // Apply position and geometry
    m_statusIndicator->setGeometry(x, y, size, size + 10);
}



void StatusWidget::ensureIndicatorOnTop()
{
    // Ensures the status indicator remains visible above other widgets.
    // Raises the indicator to the top of the widget stack if it exists.

    if (!m_statusIndicator) return;

    m_statusIndicator->raise();
}
