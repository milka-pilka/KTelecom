#ifndef NETWORKANALYZER_H
#define NETWORKANALYZER_H

#include "Measurement.h"
#include "Router.h"
//маленький класс отвечающий за состоянии роутера по показателям его метрик
class NetworkAnalyzer
{
public:
    static RouterStatus analyze(const Measurement& measurement);
};

#endif // NETWORKANALYZER_H
