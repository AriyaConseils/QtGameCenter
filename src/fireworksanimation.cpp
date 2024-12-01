#include "FireworksAnimation.h"

FireworksAnimation::FireworksAnimation(QWidget* parent)
    : QGraphicsView(parent),
    scene_(new QGraphicsScene(this)),
    timer_(new QTimer(this)),
    particleCount_(100),
    fireworksEnabled_(true) {
    setScene(scene_);
    setStyleSheet("background: transparent;");
    setRenderHint(QPainter::Antialiasing);
    setFrameStyle(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAlignment(Qt::AlignCenter);

    connect(timer_, &QTimer::timeout, this, &FireworksAnimation::updateParticles);
}


void FireworksAnimation::startAnimation(int time, const QString& text, bool enableFireworks) {
    // Starts the fireworks animation with optional text display.
    // Resets the scene, starts the particle effects if enabled,
    // and stops the animation after the specified duration.

    // Reset the scene and enable or disable fireworks
    scene_->clear();
    fireworksEnabled_ = enableFireworks;

    // Start the fireworks effect if enabled
    if (fireworksEnabled_) {
        timer_->start(50); // Update every 50 ms
        for (int i = 0; i < time / 1000; ++i) {
            QTimer::singleShot(i * 500, this, &FireworksAnimation::createFirework);
        }
    }

    // Display the winning message if text is provided
    int startMessageTimer = enableFireworks ? time / 3 : 0;
    if (!text.isEmpty()) {
        QTimer::singleShot(startMessageTimer, this, [this, text]() {
            showWinningMessage(text);
        });
    }

    // Stop the animation after the specified duration
    QTimer::singleShot(time, this, [this]() {
        timer_->stop();
        scene_->clear();
        hide();
        emit animationFinished();
    });

    // Show the animation
    show();
}


void FireworksAnimation::createFirework() {
    // Creates a firework at a random position with multiple particles.
    // Each particle has a random size, color, direction, and speed.
    // Skips creation if fireworks are disabled.

    if (!fireworksEnabled_) return;

    // Generate a random position for the firework
    int x = QRandomGenerator::global()->bounded(width());
    int y = QRandomGenerator::global()->bounded(height() / 2);

    for (int i = 0; i < particleCount_; ++i) {
        // Generate random properties for each particle
        double angle = QRandomGenerator::global()->bounded(360) * M_PI / 180.0;
        double speed = QRandomGenerator::global()->bounded(3.0) + 2.0;
        int size = QRandomGenerator::global()->bounded(3, 7);

        // Create a particle with random size and color
        QGraphicsEllipseItem* particle = new QGraphicsEllipseItem(0, 0, size, size);
        particle->setBrush(QBrush(QColor::fromRgb(
            QRandomGenerator::global()->bounded(255),
            QRandomGenerator::global()->bounded(255),
            QRandomGenerator::global()->bounded(255))));

        // Set particle data for movement and origin
        particle->setData(0, x);  // Original X position
        particle->setData(1, y);  // Original Y position
        particle->setData(2, speed * cos(angle)); // X velocity
        particle->setData(3, speed * sin(angle)); // Y velocity
        particle->setOpacity(1.0);

        // Position the particle and add it to the scene
        particle->setPos(x, y);
        scene_->addItem(particle);
    }
}


void FireworksAnimation::updateParticles() {
    // Updates the position and opacity of particles in the scene.
    // Removes particles when their opacity reaches zero.
    // Skips updates if fireworks are disabled.

    if (!fireworksEnabled_) return;

    for (QGraphicsItem* item : scene_->items()) {
        QGraphicsEllipseItem* particle = dynamic_cast<QGraphicsEllipseItem*>(item);
        if (particle) {
            // Update particle position
            double dx = particle->data(2).toDouble();
            double dy = particle->data(3).toDouble();
            particle->moveBy(dx, dy);

            // Reduce opacity and remove particle if fully transparent
            particle->setOpacity(particle->opacity() - 0.05);
            if (particle->opacity() <= 0) {
                scene_->removeItem(particle);
                delete particle;
            }
        }
    }
}


void FireworksAnimation::showWinningMessage(const QString& text) {
    // Display a winning message in the scene with scaling and fade-in/out animations.

    // Add the text message to the scene and configure its appearance
    QGraphicsTextItem* textItem = scene_->addText(text);
    QFont font("Arial", 10, QFont::Bold); // Small initial size for scaling effect
    textItem->setFont(font);
    textItem->setDefaultTextColor(Qt::red);

    // Center the text in the scene
    QRectF textRect = textItem->boundingRect();
    textItem->setPos((scene_->width() - textRect.width()) / 2,
                     (scene_->height() - textRect.height()) / 2);

    // Animate the text to grow (scaling effect)
    QPropertyAnimation* scaleAnimation = new QPropertyAnimation(textItem, "scale", this);
    scaleAnimation->setDuration(2000);
    scaleAnimation->setStartValue(0.5);
    scaleAnimation->setEndValue(3.0);
    scaleAnimation->setEasingCurve(QEasingCurve::OutBounce);
    scaleAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    // Animate the text to fade in and out
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect();
    textItem->setGraphicsEffect(effect);

    QPropertyAnimation* opacityAnimation = new QPropertyAnimation(effect, "opacity", this);
    opacityAnimation->setDuration(2000);
    opacityAnimation->setStartValue(0.0);
    opacityAnimation->setKeyValueAt(0.5, 1.0); // Fully visible at midpoint
    opacityAnimation->setEndValue(0.0);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

