#include "Measurement.h"

//собираем конструктор а также берем данные с генератора динамики нашей сети и инфы с роутера
Measurement::Measurement(
    int routerId,
    double ping,
    double downloadSpeed,
    double uploadSpeed,
    double packetLoss,
    double jitter,
    const QDateTime& timestamp
    )
    : m_routerId(routerId),
    m_ping(ping),
    m_downloadSpeed(downloadSpeed),
    m_uploadSpeed(uploadSpeed),
    m_packetLoss(packetLoss),
    m_jitter(jitter)
{
}

double Measurement::getdownloadSpeed() const
{
    return m_downloadSpeed;
}
int Measurement::getrouterId() const
{
    return m_routerId;
}
double Measurement::getpacketLoss() const
{
    return m_packetLoss;
}
double Measurement::getPing() const
{
    return m_ping;
}
QDateTime Measurement::getTimestamp() const
{
    return m_timestamp;
}
double Measurement::getJitter() const
{
    return m_jitter;
}
double Measurement::getuploadSpeed() const
{
    return m_uploadSpeed;
}
