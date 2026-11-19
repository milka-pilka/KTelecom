
#include "ScenarioController.h"

ScenarioController::ScenarioController()
    : m_step(0)
{
}

double ScenarioController::nextSeverity()
{
    // Определяем положение внутри 40-шагового цикла
    int step = m_step % 40;

    m_step++;

    // 0–9: нормальная работа
    if (step < 10)
    {
        return 0.0;
    }

    // 10–19: постепенное ухудшение
    if (step < 20)
    {
        return (step - 9) / 10.0;
    }

    // 20–29: критическое состояние
    if (step < 30)
    {
        return 1.0;
    }

    // 30–39: постепенное восстановление
    return 1.0 - (step - 29) / 10.0;
}
