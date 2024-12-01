#ifndef FIREWORKSANIMATION_H
#define FIREWORKSANIMATION_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QRandomGenerator>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

/**
 * @class FireworksAnimation
 * @brief A class to display fireworks and a winning message.
 */
class FireworksAnimation : public QGraphicsView {
    Q_OBJECT

public:
    explicit FireworksAnimation(QWidget* parent = nullptr);

    /**
     * @brief Starts the animation.
     * @param time Total duration of the animation in milliseconds.
     * @param text Text to display (optional).
     * @param enableFireworks Enables or disables fireworks.
     */
    void startAnimation(int time, const QString& text = QString(), bool enableFireworks = true);

signals:
    void animationFinished();

private:
    QGraphicsScene* scene_; ///< The graphics scene for the animation
    QTimer* timer_; ///< Timer to update the particles
    int particleCount_; ///< Number of particles per firework
    bool fireworksEnabled_; ///< Indicates whether fireworks are enabled

    /**
     * @brief Creates a firework with random particles.
     *
     * This method generates a firework at a random position in the scene.
     * Each firework consists of particles with random colors, directions,
     * and speeds.
     */
    void createFirework();

    /**
     * @brief Updates the position and opacity of particles.
     *
     * This method is called periodically to move particles based on their
     * velocity and reduce their opacity. Particles are removed when they
     * become fully transparent.
     */
    void updateParticles();

    /**
     * @brief Displays a winning message with animations.
     *
     * This method adds a text message to the scene, centers it, and
     * applies scaling and fade-in/fade-out animations for visual impact.
     *
     * @param text The text to display as the winning message.
     */
    void showWinningMessage(const QString& text);
};

#endif // FIREWORKSANIMATION_H
