#ifndef PLAYERSTATISTICS_H
#define PLAYERSTATISTICS_H

#include <QWidget>
#include "framework/helpers.h"

namespace Ui {
class PlayerStatistics;
}

/**
 * @class PlayerStatistics
 * @brief A widget for displaying player statistics and information.
 *
 * This class provides a graphical representation of a player's details,
 * including their name, games played, games won, online status, and more.
 * It also allows customization of the layout and provides interaction
 * through buttons.
 */
class PlayerStatistics : public QWidget
{
    Q_OBJECT
    DECLARE_PROPERTY(bool, online, setOnline, false) ///< Indicates whether the player is online
    DECLARE_PROPERTY(bool, fullSize, setFullSize, false) ///< Indicates whether the widget is in full size
    DECLARE_PROPERTY(QString, firstName, setFirstName, QStringLiteral("Unknown")) ///< Player's first name
    DECLARE_PROPERTY(QString, lastName, setLastName, QStringLiteral("Unknown")) ///< Player's last name
    DECLARE_PROPERTY(int, gamesPlayed, setGamesPlayed, 0) ///< Total number of games played
    DECLARE_PROPERTY(int, gamesWon, setGamesWon, 0) ///< Total number of games won
    DECLARE_PROPERTY(int, luckoMeter, setLuckoMeter, 0) ///< Player's luck meter
    DECLARE_PROPERTY(QString, image, setImage, QStringLiteral("")) ///< Path to the player's image

public:
    /**
     * @brief Constructs a PlayerStatistics widget.
     * @param parent Parent widget (optional).
     */
    explicit PlayerStatistics(QWidget *parent = nullptr);

    /**
     * @brief Destructor for the PlayerStatistics class.
     */
    ~PlayerStatistics();

    /**
     * @enum LayoutOrder
     * @brief Defines possible layout arrangements for the widget.
     *
     * - DefaultOrder: Layout with the command area, player info, and picture in a default order.
     * - AlternateOrder: Layout with the picture and player info swapped.
     */
    enum LayoutOrder {
        DefaultOrder,  ///< Default layout order
        AlternateOrder ///< Alternate layout order
    };

    /**
     * @brief Changes the layout order of the widget.
     * @param order The desired layout order (DefaultOrder or AlternateOrder).
     */
    void changeLayoutOrder(LayoutOrder order);

signals:
    void onlineChanged(); ///< Emitted when the online status changes
    void fullSizeChanged(); ///< Emitted when the full size mode changes
    void lastNameChanged(); ///< Emitted when the last name changes
    void firstNameChanged(); ///< Emitted when the first name changes
    void gamesPlayedChanged(); ///< Emitted when the games played count changes
    void gamesWonChanged(); ///< Emitted when the games won count changes
    void imageChanged(); ///< Emitted when the player's image changes
    void luckoMeterChanged(); ///< Emitted when the luck meter changes

private slots:
    /**
     * @brief Toggles the visibility of the player's main photo and information.
     *
     * This slot checks whether the player's main photo can be made visible again
     * and toggles the visibility of the associated information when the button is clicked.
     */
    void on_pushButtonHideInformations_clicked();

    /**
     * @brief Recalculates and updates the LuckoMeter value.
     *
     * The LuckoMeter value is calculated based on the player's number of games won
     * and games played, providing an indication of their "luck" or performance ratio.
     */
    void updateLukoMeter();


private:
    Ui::PlayerStatistics *ui; ///< UI instance for the widget

    /**
     * @brief Generates a stylesheet for the status display.
     * @param color The color to apply in the stylesheet.
     * @return The generated stylesheet as a QString.
     */
    QString getStatusStyleSheet(const QString &color) const;
};

#endif // PLAYERSTATISTICS_H
