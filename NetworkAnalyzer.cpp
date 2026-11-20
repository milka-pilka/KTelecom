#include "NetworkAnalyzer.h"

RouterStatus NetworkAnalyzer::analyze(const Measurement& measurement)
{
    //МЕТРИКИ ПО КОТОРЫМ МЫ РАЗБИВАЕМ РОУТЕРЫ НА 3 КАТЕГОРИИ
    if (measurement.getPing() < 0 ||
        measurement.getdownloadSpeed() < 0 ||
        measurement.getuploadSpeed() < 0 ||
        measurement.getpacketLoss() < 0 ||
        measurement.getpacketLoss() > 100 ||
        measurement.getJitter() < 0)
    {
        return RouterStatus::CRITICAL;
    }

    if (measurement.getPing() > 100 ||
        measurement.getpacketLoss() > 3 ||
        measurement.getJitter() > 25)
    {
        return RouterStatus::CRITICAL;
    }

    if (measurement.getPing() > 50 ||
        measurement.getpacketLoss() > 1 ||
        measurement.getJitter() > 10)
    {
        return RouterStatus::WARNING;
    }

    return RouterStatus::OK;
}
