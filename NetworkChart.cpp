#include "NetworkChart.h"

#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <algorithm>

NetworkChart::NetworkChart(QWidget *parent)
    : QWidget(parent),
    m_title("NETWORK CHART"),
    m_unit(""),
    m_lineColor("#24B6A3")
{
    setMinimumHeight(220);
    setMinimumWidth(300);
}

void NetworkChart::setData(const QVector<double>& data)
{
    m_data = data;
    update();
}

void NetworkChart::setTimeData(const QVector<QDateTime>& timeData)
{
    m_timeData = timeData;
    update();
}

void NetworkChart::setTitle(const QString& title)
{
    m_title = title;
    update();
}

void NetworkChart::setUnit(const QString& unit)
{
    m_unit = unit;
    update();
}

void NetworkChart::setLineColor(const QColor& color)
{
    m_lineColor = color;
    update();
}

void NetworkChart::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Цвета интерфейса
    QColor background("#1B2631");
    QColor gridColor("#34495A");
    QColor textColor("#8295A5");

    painter.fillRect(rect(), background);

    // Заголовок
    painter.setPen(QColor("#E5EDF3"));
    QFont titleFont = painter.font();
    titleFont.setBold(true);
    titleFont.setPointSize(11);
    painter.setFont(titleFont);

    painter.drawText(
        QRect(16, 10, width() - 32, 25),
        Qt::AlignLeft | Qt::AlignVCenter,
        m_title
        );

    // Область графика
    QRectF chartArea(
        55,
        48,
        width() - 85,
        height() - 100
        );

    if (chartArea.width() <= 0 || chartArea.height() <= 0)
        return;

    // 1. Сначала проверяем, есть ли данные
    if (m_data.isEmpty())
    {
        painter.setPen(textColor);
        painter.drawText(
            chartArea,
            Qt::AlignCenter,
            "WAITING FOR DATA..."
            );
        return;
    }

    // 2. Сразу определяем диапазон значений (до отрисовки сетки)
    auto minMax = std::minmax_element(
        m_data.begin(),
        m_data.end()
        );

    double minValue = *minMax.first;
    double maxValue = *minMax.second;

    if (maxValue - minValue < 0.001)
    {
        minValue -= 1.0;
        maxValue += 1.0;
    }

    double padding = (maxValue - minValue) * 0.15;

    minValue = std::max(0.0, minValue - padding);
    maxValue += padding;

    // 3. Настраиваем мелкий шрифт для всех подписей осей
    QFont labelFont = painter.font();
    labelFont.setBold(false);
    labelFont.setPointSize(8);
    painter.setFont(labelFont);

    // 4. Горизонтальная сетка и подписи значений по оси Y
    for (int i = 0; i <= 4; i++)
    {
        double ratio = i / 4.0;
        double y = chartArea.top() + chartArea.height() * ratio;

        painter.setPen(QPen(gridColor, 1));
        painter.drawLine(
            QPointF(chartArea.left(), y),
            QPointF(chartArea.right(), y)
            );

        double value = maxValue - ratio * (maxValue - minValue);

        painter.setPen(textColor);
        painter.drawText(
            QRectF(2, y - 9, 48, 18),
            Qt::AlignRight | Qt::AlignVCenter,
            QString::number(value, 'f', 1)
            );
    }

    // Строим линию графика
    QPainterPath path;

    for (int i = 0; i < m_data.size(); i++)
    {
        double x = chartArea.left();

        if (m_data.size() > 1)
        {
            x += chartArea.width() * i / (m_data.size() - 1);
        }

        double normalized = (m_data[i] - minValue) / (maxValue - minValue);
        double y = chartArea.bottom() - normalized * chartArea.height();

        if (i == 0)
            path.moveTo(x, y);
        else
            path.lineTo(x, y);
    }

    // Отрисовываем линию
    painter.setPen(
        QPen(m_lineColor, 2.5, Qt::SolidLine,
             Qt::RoundCap, Qt::RoundJoin)
        );
    painter.drawPath(path);

    // Подписи временной шкалы по оси X
    painter.setPen(textColor);

    if (!m_timeData.isEmpty())
    {
        int labelCount = std::min(4, static_cast<int>(m_timeData.size()));

        for (int i = 0; i < labelCount; i++)
        {
            int index = (m_timeData.size() - 1) * i /
                        std::max(1, labelCount - 1);

            double x = chartArea.left();

            if (m_timeData.size() > 1)
            {
                x += chartArea.width() * index /
                     (m_timeData.size() - 1);
            }

            QString timeText = m_timeData[index].toString("HH:mm:ss");

            painter.drawText(
                QRectF(x - 35, chartArea.bottom() + 12, 70, 18),
                Qt::AlignCenter,
                timeText
                );
        }
    }

    // Единица измерения
    painter.setPen(textColor);
    painter.drawText(
        QRectF(
            chartArea.right() - 80,
            chartArea.bottom() + 10,
            80,
            18
            ),
        Qt::AlignRight,
        m_unit
        );
}
