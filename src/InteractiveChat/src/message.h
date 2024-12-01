#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QVariantList>

class Message
{
public:
    Message(const QString &text, const QString &timestamp, const QString &role);

    QString text() const;
    QString timestamp() const;
    QString role() const;
    QVariantList styleParams() const;

    void setStyleParams(const QVariantList &styleParams);

private:
    QString m_text;
    QString m_timestamp;
    QString m_role;
    QVariantList m_styleParams;
};

#endif // MESSAGE_H
