
#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include "Router.h"
#include "Measurement.h"
#include "TelemetryGenerator.h"
#include "ScenarioController.h"

#include <vector>

class NetworkManager
{
public:
    NetworkManager();

    // Обновление всех маршрутизаторов
    void update();

    // Получение списка маршрутизаторов
    const std::vector<Router>& getRouters() const;

    // Получение последних измерений
    const std::vector<Measurement>& getLatestMeasurements() const;
public:
    const std::vector<std::vector<Measurement>>&
    getMeasurementHistory() const;


private:
    std::vector<Router> m_routers;

    std::vector<TelemetryGenerator> m_generators;

    std::vector<ScenarioController> m_controllers;

    std::vector<Measurement> m_latestMeasurements;
private:
    std::vector<std::vector<Measurement>> m_measurementHistory;
};

#endif // NETWORKMANAGER_H
