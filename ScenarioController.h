
#ifndef SCENARIOCONTROLLER_H
#define SCENARIOCONTROLLER_H

class ScenarioController
{
public:
    ScenarioController();

    double nextSeverity();

private:
    int m_step;
};

#endif // SCENARIOCONTROLLER_H
