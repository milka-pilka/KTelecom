#ifndef ALERT_H
#define ALERT_H

#include <QString>
#include <QDateTime>

class Alert
{
public:
    Alert(
        int routerId,
        const QString& routerName,
        const QString& event,
        const QString& severity,
        const QString& description
        );

    int getRouterId() const;
    QString getRouterName() const;
    QString getEvent() const;
    QString getSeverity() const;
    QString getDescription() const;
    QDateTime getTimestamp() const;

private:
    int m_routerId;
    QString m_routerName;
    QString m_event;
    QString m_severity;
    QString m_description;
    QDateTime m_timestamp;
};

#endif // ALERT_H