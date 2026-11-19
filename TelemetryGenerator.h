
#ifndef TELEMETRYGENERATOR_H
#define TELEMETRYGENERATOR_H

#include "Measurement.h"
#include "GarchProcess.h"

#include <random>

enum class TelemetryScenario
{
    NORMAL,
    CONGESTION,
    DEGRADATION,
    RECOVERY
};

class TelemetryGenerator
{
public:
    TelemetryGenerator(
        int routerId,
        double basePing,
        double baseJitter,
        double baseDownload,
        double baseUpload,
        unsigned int seed
        );

    Measurement nextMeasurement();

    void setSeverity(double severity);

private:
    int m_routerId;

    double m_basePing;
    double m_baseJitter;
    double m_baseDownload;
    double m_baseUpload;
    double m_severity;

    void setScenario(TelemetryScenario scenario);


    GarchProcess m_pingGarch;
    GarchProcess m_jitterGarch;

    std::mt19937 m_engine;

    TelemetryScenario m_scenario;
};

#endif // TELEMETRYGENERATOR_H
