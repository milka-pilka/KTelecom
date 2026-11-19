#include "Router.h"

// Конструктор
Router::Router(
    int id,
    const QString& name,
    const QString& location,
    double latitude,
    double longitude,
    RouterStatus status
    )
    : m_id(id),
    m_name(name),
    m_location(location),
    m_latitude(latitude),
    m_longitude(longitude),
    m_status(status)
{
}

// Методы получения данных
void Router::setStatus(RouterStatus status)
{
    m_status = status;
}
int Router::getId() const
{
    return m_id;
}

QString Router::getName() const
{
    return m_name;
}

QString Router::getLocation() const
{
    return m_location;
}

double Router::getLatitude() const
{
    return m_latitude;
}

double Router::getLongitude() const
{
    return m_longitude;
}

RouterStatus Router::getStatus() const
{
    return m_status;
}
