#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <QDateTime>
//собираем всю инфу по роутеру для таблицы(всего их будет 10 для кейса)
class Measurement
{
public:
    //констркутор
    Measurement(
        int routerId,
        double ping,
        double downloadSpeed,
        double uploadSpeed,
        double packetLoss,
        double jitter,
        const QDateTime& timestamp
    );

    int getrouterId() const;
    double getPing() const;
    double getdownloadSpeed() const;
    double getuploadSpeed() const;
    double getpacketLoss() const;
    double getJitter() const;
    QDateTime getTimestamp() const;
private:
    int m_routerId;
    double m_ping;
    double m_downloadSpeed;
    double m_uploadSpeed;
    double m_packetLoss;
    double m_jitter;
    QDateTime m_timestamp;
};

#endif // MEASUREMENT_H
