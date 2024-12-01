#include "BubbleChat.h"
#include "messagedelegate.h"
#include <QVBoxLayout>
#include <QDateTime>

BubbleChat::BubbleChat(QWidget *parent)
    : QWidget(parent),
    m_userBubbleColor("#930092"),
    m_botBubbleColor("#6390FF"),
    m_timestampColor(Qt::black),
    m_textColor(Qt::white),
    m_textSize(13),
    m_timestampSize(7),
    m_backgroundColor(Qt::white)
{
    chatView = new QListView(this);
    messageModel = new MessageModel(this);


    chatView->setModel(messageModel);
    chatView->setItemDelegate(new MessageDelegate(this));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(chatView);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);
}

BubbleChat::~BubbleChat()
{
}

QColor BubbleChat::userBubbleColor() const { return m_userBubbleColor; }
void BubbleChat::setUserBubbleColor(const QColor &color) {
    if (color != m_userBubbleColor) {
        m_userBubbleColor = color;
        emit userBubbleColorChanged();
    }
}

QColor BubbleChat::botBubbleColor() const { return m_botBubbleColor; }
void BubbleChat::setBotBubbleColor(const QColor &color) {
    if (color != m_botBubbleColor) {
        m_botBubbleColor = color;
        emit botBubbleColorChanged();
    }
}

QColor BubbleChat::timestampColor() const { return m_timestampColor; }
void BubbleChat::setTimestampColor(const QColor &color) {
    if (color != m_timestampColor) {
        m_timestampColor = color;
        emit timestampColorChanged();
    }
}

QColor BubbleChat::textColor() const { return m_textColor; }
void BubbleChat::setTextColor(const QColor &color) {
    if (color != m_textColor) {
        m_textColor = color;
        emit textColorChanged();
    }
}

int BubbleChat::textSize() const { return m_textSize; }
void BubbleChat::setTextSize(int size) {
    if (size != m_textSize) {
        m_textSize = size;
        emit textSizeChanged();
    }
}

int BubbleChat::timestampSize() const { return m_timestampSize; }
void BubbleChat::setTimestampSize(int size) {
    if (size != m_timestampSize) {
        m_timestampSize = size;
        emit timestampSizeChanged();
    }
}

QColor BubbleChat::backgroundColor() const { return m_backgroundColor; }
void BubbleChat::setBackgroundColor(const QColor &color) {
    if (color != m_backgroundColor) {
        m_backgroundColor = color;
        emit backgroundColorChanged();
    }
}

void BubbleChat::sendMessage(const QString &text) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm AP");
    Message msg(text, timestamp, "user");
    QVariantList styleParams;
    styleParams << m_userBubbleColor << m_botBubbleColor << m_timestampColor << m_textColor << m_textSize << m_timestampSize << m_backgroundColor;
    msg.setStyleParams(styleParams);
    messageModel->addMessage(msg);
}

void BubbleChat::receiveMessage(const QString &text) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm AP");
    Message msg(text, timestamp, "bot");
    QVariantList styleParams;
    styleParams << m_userBubbleColor << m_botBubbleColor << m_timestampColor << m_textColor << m_textSize << m_timestampSize << m_backgroundColor;
    msg.setStyleParams(styleParams);
    messageModel->addMessage(msg);
}
