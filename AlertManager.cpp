#include "AlertManager.h"
#include "Router.h"

void AlertManager::checkRouter(const Router& router)
{
    const int routerId = router.getId();
    const RouterStatus currentStatus = router.getStatus();

    // Первое обнаружение маршрутизатора
    if (!m_previousStatuses.contains(routerId))
    {
        m_previousStatuses.insert(routerId, currentStatus);
        return;
    }

    const RouterStatus previousStatus =
        m_previousStatuses.value(routerId);//манипуляции для использование переменных в области private

    // Если состояние не изменилось, ничего не делаем
    if (previousStatus == currentStatus)
        return;
    //Объявляем переменныые строчного типа
    QString event;
    QString severity;
    QString description;

    // Определяем тип события
    if (previousStatus == RouterStatus::OK &&
        currentStatus == RouterStatus::WARNING)
    {
        event = "Performance degraded";
        severity = "WARNING";
        description = "Router performance has degraded";
    }
    else if (
        (previousStatus == RouterStatus::OK ||
         previousStatus == RouterStatus::WARNING) &&
        currentStatus == RouterStatus::CRITICAL)
    {
        event = "Critical failure detected";
        severity = "CRITICAL";
        description = "Router entered critical state";
    }
    else if (previousStatus == RouterStatus::CRITICAL &&
             currentStatus == RouterStatus::WARNING)
    {
        event = "Partial recovery";
        severity = "WARNING";
        description = "Router condition has improved";
    }
    else if (
        (previousStatus == RouterStatus::WARNING ||
         previousStatus == RouterStatus::CRITICAL) &&
        currentStatus == RouterStatus::OK)
    {
        event = "Router recovered";
        severity = "INFO";
        description = "Router returned to normal state";
    }

    // Создаём событие, если переход обработан
    if (!event.isEmpty())
    {
        //задаем переменную класса Alert, опираясь на шаблон из констркуктора Alert.h
        Alert alert(
            router.getId(),
            router.getName(),
            event,
            severity,
            description
            );

        m_alerts.append(alert);
    }

    // Запоминаем новое состояние
    m_previousStatuses[routerId] = currentStatus;//заменили переменную прошлого состояния на состоянии в данный момент
}

const QList<Alert>& AlertManager::getAlerts() const
{
    return m_alerts;
}