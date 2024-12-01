#ifndef BubbleChat_H
#define BubbleChat_H

#include <QWidget>
#include <QListView>
#include <QColor>
#include "messagemodel.h"

class BubbleChat : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor userBubbleColor READ userBubbleColor WRITE setUserBubbleColor NOTIFY userBubbleColorChanged)
    Q_PROPERTY(QColor botBubbleColor READ botBubbleColor WRITE setBotBubbleColor NOTIFY botBubbleColorChanged)
    Q_PROPERTY(QColor timestampColor READ timestampColor WRITE setTimestampColor NOTIFY timestampColorChanged)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)
    Q_PROPERTY(int textSize READ textSize WRITE setTextSize NOTIFY textSizeChanged)
    Q_PROPERTY(int timestampSize READ timestampSize WRITE setTimestampSize NOTIFY timestampSizeChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)

public:
    BubbleChat(QWidget *parent = nullptr);
    ~BubbleChat();

    QColor userBubbleColor() const;
    void setUserBubbleColor(const QColor &color);

    QColor botBubbleColor() const;
    void setBotBubbleColor(const QColor &color);

    QColor timestampColor() const;
    void setTimestampColor(const QColor &color);

    QColor textColor() const;
    void setTextColor(const QColor &color);

    int textSize() const;
    void setTextSize(int size);

    int timestampSize() const;
    void setTimestampSize(int size);

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);

public slots:
    void sendMessage(const QString &text);
    void receiveMessage(const QString &text);

signals:
    void userBubbleColorChanged();
    void botBubbleColorChanged();
    void timestampColorChanged();
    void textColorChanged();
    void textSizeChanged();
    void timestampSizeChanged();
    void backgroundColorChanged();

private:
    QListView *chatView;
    MessageModel *messageModel;
    QColor m_userBubbleColor;
    QColor m_botBubbleColor;
    QColor m_timestampColor;
    QColor m_textColor;
    int m_textSize;
    int m_timestampSize;
    QColor m_backgroundColor;
};

#endif // BubbleChat_H
