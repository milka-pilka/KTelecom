
#include "TelemetryGenerator.h"

#include <QDateTime>

#include <algorithm>

TelemetryGenerator::TelemetryGenerator(
    int routerId,
    double basePing,
    double baseJitter,
    double baseDownload,
    double baseUpload,
    unsigned int seed
    )
    : m_routerId(routerId),
    m_basePing(basePing),
    m_baseJitter(baseJitter),
    m_baseDownload(baseDownload),
    m_baseUpload(baseUpload),

    m_pingGarch(0.5, 0.10, 0.85, seed),
    m_jitterGarch(0.08, 0.10, 0.85, seed + 1),

    m_engine(seed + 2),

    m_severity(0.0)
{
}
void TelemetryGenerator::setSeverity(double severity)
{
    m_severity = std::clamp(severity, 0.0, 1.0);
}
void TelemetryGenerator::setScenario(TelemetryScenario scenario)
{
    m_scenario = scenario;
}


Measurement TelemetryGenerator::nextMeasurement()
{
    // Степень ухудшения сети
    double severity = m_severity;

    // Изменение базовых показателей
    double pingOffset = 70.0 * severity;
    double jitterOffset = 18.0 * severity;

    // Потери пакетов
    double lossMin = 0.0;
    double lossMax = 0.5 + 5.5 * severity;

    // Изменение скорости
    double speedMin = 0.90 - 0.70 * severity;
    double speedMax = 1.10 - 0.60 * severity;

    // Создаём распределения
    std::uniform_real_distribution<double> packetLossDistribution(
        lossMin, lossMax
        );

    std::uniform_real_distribution<double> speedDistribution(
        speedMin, speedMax
        );

    // Генерируем Ping и Jitter
    double ping =
        m_basePing + pingOffset + m_pingGarch.next();

    double jitter =
        m_baseJitter + jitterOffset + m_jitterGarch.next();

    // Генерируем потери пакетов
    double packetLoss =
        packetLossDistribution(m_engine);

    // Генерируем скорость
    double download =
        m_baseDownload * speedDistribution(m_engine);

    double upload =
        m_baseUpload * speedDistribution(m_engine);

    // Ограничиваем значения
    ping = std::max(1.0, ping);
    jitter = std::max(0.0, jitter);

    download = std::max(0.0, download);
    upload = std::max(0.0, upload);

    return Measurement(
        m_routerId,
        ping,
        download,
        upload,
        packetLoss,
        jitter,
        QDateTime::currentDateTime()
        );
}

