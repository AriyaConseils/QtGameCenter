#include "message.h"

Message::Message(const QString &text, const QString &timestamp, const QString &role)
    : m_text(text), m_timestamp(timestamp), m_role(role)
{
}

QString Message::text() const
{
    return m_text;
}

QString Message::timestamp() const
{
    return m_timestamp;
}

QString Message::role() const
{
    return m_role;
}

QVariantList Message::styleParams() const
{
    return m_styleParams;
}

void Message::setStyleParams(const QVariantList &styleParams)
{
    m_styleParams = styleParams;
}
