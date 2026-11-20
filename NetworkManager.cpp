#include "NetworkAnalyzer.h"
#include "NetworkManager.h"
#include <array>


namespace
{
constexpr std::size_t MAX_HISTORY_SIZE = 120;

struct RouterLocation
{
    QString address;
    double latitude;
    double longitude;
};
//задаем адреса
const std::array<RouterLocation, 10> ROUTER_LOCATIONS =
    {{
        {"Проспект Космонавтов", 56.901746, 60.613991},
        {"Уралмаш",              56.888760, 60.613292},
        {"Эльмаш",               56.892700, 60.630570},
        {"ВИЗ",                  56.858230, 60.554120},
        {"Железнодорожный вокзал", 56.858890, 60.604170},
        {"Площадь 1905 года",    56.836870, 60.599096},
        {"Геологическая",        56.826820, 60.602477},
        {"Ботаническая",         56.797300, 60.632549},
        {"Вторчермет",           56.767460, 60.615170},
        {"Кольцово",             56.751940, 60.805660}
    }};
}


NetworkManager::NetworkManager()
{
    m_measurementHistory.resize(10);
    // Создаём 10 виртуальных маршрутизаторов
    for (int i = 0; i < 10; i++)
    {
        int id = i + 1;

        // Названия устройств
        QString name = QString("Router %1").arg(id);

        const RouterLocation& point = ROUTER_LOCATIONS[i];

        QString location = point.address;

        // Создаём маршрутизатор
        m_routers.emplace_back(
            id,
            name,
            location,
            point.latitude,
            point.longitude,
            RouterStatus::OK
            );

        // Создаём генератор телеметрии
        m_generators.emplace_back(
            id,
            25.0 + i * 2.0,   // Ping
            2.0 + i * 0.2,    // Jitter
            100.0 + i * 5.0,  // Download
            50.0 + i * 2.0,   // Upload
            42 + i * 100      // Seed
            );

        // Создаём контроллер сценария
        m_controllers.emplace_back();

        // Создаём первое измерение
        m_latestMeasurements.push_back(
            m_generators.back().nextMeasurement()
            );
        m_measurementHistory[i].push_back(
            m_latestMeasurements.back()
            );
    }
}
//постоянно обновляем данные о маршрутизаторах
void NetworkManager::update()
{
    // Проходим по всем маршрутизаторам
    for (std::size_t i = 0; i < m_routers.size(); i++)
    {
        // Получаем степень ухудшения
        double severity =
            m_controllers[i].nextSeverity();

        // Передаём её генератору
        m_generators[i].setSeverity(severity);

        // Генерируем новое измерение
        Measurement measurement =
            m_generators[i].nextMeasurement();

        // Анализируем показатели
        RouterStatus status =
            NetworkAnalyzer::analyze(measurement);

        // Обновляем статус маршрутизатора
        m_routers[i].setStatus(status);

        // Сохраняем последнее измерение
        m_latestMeasurements[i] = measurement;

        m_measurementHistory[i].push_back(measurement);

        // Ограничиваем размер истории
        if (m_measurementHistory[i].size() > MAX_HISTORY_SIZE)
        {
            m_measurementHistory[i].erase(
                m_measurementHistory[i].begin()
                );
        }
    }
}

const std::vector<Router>& NetworkManager::getRouters() const
{
    return m_routers;
}

const std::vector<Measurement>&
NetworkManager::getLatestMeasurements() const
{
    return m_latestMeasurements;
}
const std::vector<std::vector<Measurement>>&NetworkManager::getMeasurementHistory() const
{
    return m_measurementHistory;
}