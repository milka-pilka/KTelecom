#include "GarchProcess.h"

GarchProcess::GarchProcess(
    double omega,
    double alpha,
    double beta,
    unsigned int seed
)
    : m_omega(omega),
    m_alpha(alpha),
    m_beta(beta),
    m_previousResidual(0.0),
    m_previousVariance(0.0),
    m_engine(seed),
    m_normalDistribution(0.0, 1.0)
{
    if (!std::isfinite(omega) ||
        !std::isfinite(alpha) ||
        !std::isfinite(beta) ||
        omega <= 0.0 ||
        alpha < 0.0 ||
        beta < 0.0 ||
        alpha + beta >= 1.0)
    {
        throw std::invalid_argument(
            "Invalid GARCH parameters"
            );
    }

    // Начальная оценка дисперсии
    m_previousVariance =
        m_omega / (1.0 - m_alpha - m_beta);
}

double GarchProcess::next()
{
    //дикая форула нашей функции
    double variance =
        m_omega
        + m_alpha * m_previousResidual * m_previousResidual
        + m_beta * m_previousVariance;

    // Генерируем случайное число N(0, 1)
    double z = m_normalDistribution(m_engine);

    // Рассчитываем случайное отклонение
    double residual = std::sqrt(variance) * z;

    // Сохраняем значения для следующей итерации
    m_previousVariance = variance;
    m_previousResidual = residual;

    return residual;
}
