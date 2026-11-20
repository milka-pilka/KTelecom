#ifndef ALERTMANAGER_H
#define ALERTMANAGER_H

#include "Alert.h"
#include "Router.h"

#include <QHash>
#include <QList>

class AlertManager
{
public:
    // Проверка изменения состояния маршрутизатора
    void checkRouter(const Router& router);

    // Получение истории оповещений
    const QList<Alert>& getAlerts() const;

private:
    // Последние известные статусы маршрутизаторов
    QHash<int, RouterStatus> m_previousStatuses;//словарь: ключ - значение;хэш таблица одним словом

    // История оповещений
    QList<Alert> m_alerts;
};

#endif // ALERTMANAGER_H