#ifndef GARCHPROCESS_H
#define GARCHPROCESS_H

#include<random>
//Класс отвечающий за нашу задающую функцию, создание зерна сида, а также ско
class GarchProcess
{
public:
    //конструктор
    GarchProcess(
        double omega,
        double alpha,
        double beta,
        unsigned int seed
    );

    double next();
private:
    double m_omega;
    double m_alpha;
    double m_beta;
    double m_previousResidual;
    double m_previousVariance;

    std::mt19937 m_engine;//генератор случайных значений

    std::normal_distribution<double> m_normalDistribution;

};

#endif // GARCHPROCESS_H
