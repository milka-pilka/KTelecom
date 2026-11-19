#ifndef NETWORKCHART_H
#define NETWORKCHART_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QColor>
#include <QDateTime>

class NetworkChart : public QWidget
{
public:
    explicit NetworkChart(QWidget *parent = nullptr);

    void setData(const QVector<double>& data);
    void setTitle(const QString& title);
    void setUnit(const QString& unit);
    void setLineColor(const QColor& color);
    void setTimeData(const QVector<QDateTime>& timeData);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<double> m_data;

    QString m_title;
    QString m_unit;

    QColor m_lineColor;

    QVector<QDateTime> m_timeData;
};



#endif // NETWORKCHART_H

