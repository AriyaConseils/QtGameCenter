#ifndef STATUSWIDGET_H
#define STATUSWIDGET_H

#include <QWidget>
#include <QLabel>
#include "framework/helpers.h"
/**
 * @class StatusWidget
 * @brief A widget that displays an online/offline status indicator.
 *
 * This widget shows a circular indicator (green for online, red for offline)
 * that adjusts its size and position dynamically based on the widget's size.
 */
class StatusWidget : public QWidget {
    Q_OBJECT

    /**
     * @brief Sets the online status.
     *
     * Updates the indicator color based on the online status
     * and emits the `onlineChanged` signal.
     *
     * @param online True for online, false for offline.
     */
    DECLARE_PROPERTY_CB(bool, online, setOnline, false) {
        Q_UNUSED(_online)
        updateIndicator();
        ensureIndicatorOnTop();
    }

public:
    /**
     * @brief Constructs a StatusWidget.
     * @param parent The parent widget (optional).
     */
    explicit StatusWidget(QWidget *parent = nullptr);

    /**
     * @brief Destructor for the StatusWidget.
     */
    ~StatusWidget();



signals:
    /**
     * @brief Signal emitted when the online status changes.
     * @param online The updated online status (true for online, false for offline).
     */
    void onlineChanged();

protected:
    /**
     * @brief Handles resize events for the widget.
     *
     * Updates the size and position of the status indicator
     * when the widget is resized.
     *
     * @param event The resize event details.
     */
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel *m_statusIndicator; ///< The status indicator label

    /**
     * @brief Updates the status indicator's appearance and position.
     *
     * Adjusts the size, color, and position of the indicator
     * based on the current widget dimensions and online status.
     */
    void updateIndicator();

    /**
     * @brief Ensures the status indicator is always on top.
     *
     * Raises the status indicator to make it visible above other widgets.
     */
    void ensureIndicatorOnTop();
};

#endif // STATUSWIDGET_H
