#include "Alert.h"

Alert::Alert(
    int routerId,
    const QString& routerName,
    const QString& event,
    const QString& severity,
    const QString& description
    )
    : m_routerId(routerId)
    , m_routerName(routerName)
    , m_event(event)
    , m_severity(severity)
    , m_description(description)
    , m_timestamp(QDateTime::currentDateTime())
{
}

int Alert::getRouterId() const
{
    return m_routerId;
}

QString Alert::getRouterName() const
{
    return m_routerName;
}

QString Alert::getEvent() const
{
    return m_event;
}

QString Alert::getSeverity() const
{
    return m_severity;
}

QString Alert::getDescription() const
{
    return m_description;
}

QDateTime Alert::getTimestamp() const
{
    return m_timestamp;
}