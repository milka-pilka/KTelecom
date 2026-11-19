
#ifndef ROUTER_H
#define ROUTER_H

#include <QString>

// Возможные состояния маршрутизатора
enum class RouterStatus
{
    OK,
    WARNING,
    CRITICAL
};

class Router
{
public:
    // Конструктор
    Router(
        int id,
        const QString& name,
        const QString& location,
        double latitude,
        double longitude,
        RouterStatus status
        );

    // Методы получения данных
    void setStatus(RouterStatus status);
    int getId() const;
    QString getName() const;
    QString getLocation() const;

    double getLatitude() const;
    double getLongitude() const;

    RouterStatus getStatus() const;

private:
    // Данные маршрутизатора
    int m_id;
    QString m_name;
    QString m_location;

    double m_latitude;
    double m_longitude;

    RouterStatus m_status;
};

#endif // ROUTER_H

