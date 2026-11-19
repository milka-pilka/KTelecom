#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QList>
#include <QStyle>
#include <QTimer>
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QColor>
#include "NetworkChart.h"
#include <QVBoxLayout>
#include <QVector>
#include <QDateTime>
#include <QComboBox>
#include <QUrl>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QApplication>
#include <QTimer>
#include <QFile>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Загружаем корпоративную тему К-Телеком
    QFile themeFile(":/new/prefix1/theme.qss");

    if (themeFile.open(QFile::ReadOnly | QFile::Text))
    {
        QString styleSheet = QString::fromUtf8(themeFile.readAll());

        qApp->setStyleSheet(styleSheet);

        themeFile.close();
    }
    else
    {
        qDebug() << "Не удалось загрузить theme.qss:"
                 << themeFile.errorString();
    }

    // Настройка таблицы оповещений
    ui->alertsTableWidget->setColumnCount(5);

    ui->alertsTableWidget->setHorizontalHeaderLabels({
        "Time",
        "Router",
        "Event",
        "Severity",
        "Description"
    });

    ui->alertsTableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->alertsTableWidget->setSelectionBehavior(
        QAbstractItemView::SelectRows
        );

    ui->alertsTableWidget->setEditTriggers(
        QAbstractItemView::NoEditTriggers
        );

    ui->alertsTableWidget->setAlternatingRowColors(false);
    ui->alertsTableWidget->verticalHeader()->setVisible(false);

    // Создаём встроенный браузер для географической карты
    m_mapView = new QWebEngineView(ui->mapContainer);

    // Добавляем браузер в layout контейнера
    ui->mapContainerLayout->addWidget(m_mapView);

    m_mapView->load(QUrl("qrc:/new/prefix1/map.html"));

    // Ожидаем завершения загрузки HTML-карты
    connect(
        m_mapView,
        &QWebEngineView::loadFinished,
        this,
        [this](bool success)
        {
            if (!success)
            {
                qDebug() << "Map loading failed!";
                return;
            }

            m_mapReady = true;

            qDebug() << "Map loaded successfully!";

            updateMap();
        }
        );

    ui->routerSelector->addItem("Все маршрутизаторы");

    for (int i = 1; i <= 10; i++)
    {
        ui->routerSelector->addItem(
            QString("Router %1").arg(i)
            );
    }

    connect(
        ui->routerSelector,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        [this](int index)
        {
            Q_UNUSED(index);
            updateAnalyticsChart();
        }
        );

    m_pingChart = new NetworkChart(ui->pingChartWidget);

    // Создаём компоновку внутри контейнера
    QVBoxLayout* pingLayout =
        new QVBoxLayout(ui->pingChartWidget);

    // Убираем внутренние отступы
    pingLayout->setContentsMargins(0, 0, 0, 0);

    // Добавляем график в контейнер
    pingLayout->addWidget(m_pingChart);

    // Настраиваем график
    m_pingChart->setTitle("AVERAGE NETWORK PING");
    m_pingChart->setUnit("ms");
    m_pingChart->setLineColor(QColor("#C82058"));

    QList<QPushButton*> navigationButtons = {
        ui->dashboardButton,
        ui->routersButton,
        ui->analyticsButton,
        ui->mapButton,
        ui->alertsButton,
        ui->reportsButton
    };

    // Подключаем все кнопки к единому механизму
    for (int i = 0; i < navigationButtons.size(); ++i)
    {
        QPushButton* button = navigationButtons[i];

        connect(button, &QPushButton::clicked, this,
                [this, navigationButtons, i]()
                {
                    // Переключаем страницу
                    ui->stackedWidget->setCurrentIndex(i);

                    // Обновляем состояние всех кнопок
                    for (QPushButton* btn : navigationButtons)
                    {
                        btn->setProperty("active", false);

                        btn->style()->unpolish(btn);
                        btn->style()->polish(btn);
                    }

                    // Выделяем активную кнопку
                    QPushButton* activeButton = navigationButtons[i];

                    activeButton->setProperty("active", true);

                    activeButton->style()->unpolish(activeButton);
                    activeButton->style()->polish(activeButton);
                });
    }

    // При запуске активна первая страница
    navigationButtons[0]->setProperty("active", true);
    navigationButtons[0]->style()->unpolish(navigationButtons[0]);
    navigationButtons[0]->style()->polish(navigationButtons[0]);

    // Настройка таблицы маршрутизаторов

    ui->routersTableWidget->setColumnCount(9);

    ui->routersTableWidget->setHorizontalHeaderLabels({
        "ID",
        "ROUTER",
        "LOCATION",
        "PING (ms)",
        "JITTER (ms)",
        "PACKET LOSS (%)",
        "DOWNLOAD (Mbps)",
        "UPLOAD (Mbps)",
        "STATUS"
    });

    ui->routersTableWidget->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->routersTableWidget->horizontalHeader()
        ->setStretchLastSection(true);

    ui->routersTableWidget->setAlternatingRowColors(true);
    ui->routersTableWidget->setEditTriggers(
        QAbstractItemView::NoEditTriggers
        );

    ui->routersTableWidget->setSelectionBehavior(
        QAbstractItemView::SelectRows
        );

    ui->routersTableWidget->setSelectionMode(
        QAbstractItemView::SingleSelection
        );

    // Первоначальное обновление данных
    m_networkManager.update();

    updateDashboard();
    updateRoutersTable();

    updateAnalyticsChart();

    // Создаём таймер
    m_updateTimer = new QTimer(this);

    // Обновляем данные каждую секунду
    connect(
        m_updateTimer,
        &QTimer::timeout,
        this,
        [this]()
        {
            m_networkManager.update();

            updateDashboard();
            updateRoutersTable();
            updateAnalyticsChart();
            updateMap();
            updateMapCards();
            checkAlerts();
        }
        );

    // Запускаем таймер
    m_updateTimer->start(1000);

}
void MainWindow::updateDashboard()
{
    const auto& routers = m_networkManager.getRouters();

    int okCount = 0;
    int warningCount = 0;
    int criticalCount = 0;

    for (const Router& router : routers)
    {
        switch (router.getStatus())
        {
        case RouterStatus::OK:
            okCount++;
            break;

        case RouterStatus::WARNING:
            warningCount++;
            break;

        case RouterStatus::CRITICAL:
            criticalCount++;
            break;
        }
    }

    ui->totalRoutersValue->setText(
        QString::number(routers.size())
        );

    ui->okRoutersValue->setText(
        QString::number(okCount)
        );

    ui->mediumRoutersValue->setText(
        QString::number(warningCount)
        );

    ui->criticalRoutersValue->setText(
        QString::number(criticalCount)
        );
}
void MainWindow::updateAnalyticsChart()
{
    const auto& history =
        m_networkManager.getMeasurementHistory();

    if (history.empty() || m_pingChart == nullptr)
        return;

    QVector<double> pingValues;
    QVector<QDateTime> timeValues;

    // Получаем выбранный элемент списка
    int selectedIndex = ui->routerSelector->currentIndex();
    if (selectedIndex < 0)
        return;

    // Вариант 1: средний Ping по всем маршрутизаторам
    if (selectedIndex == 0)
    {
        std::size_t count = history[0].size();

        for (std::size_t i = 0; i < count; i++)
        {
            double pingSum = 0.0;
            int routerCount = 0;

            for (const auto& routerHistory : history)
            {
                if (i < routerHistory.size())
                {
                    pingSum += routerHistory[i].getPing();
                    routerCount++;
                }
            }

            if (routerCount > 0)
            {
                double averagePing = pingSum / routerCount;
                pingValues.push_back(averagePing);

                timeValues.push_back(
                    history[0][i].getTimestamp()
                    );
            }
        }

        m_pingChart->setTitle("AVERAGE PING — ALL ROUTERS");
    }

    // Вариант 2: Ping конкретного маршрутизатора
    else
    {
        int routerIndex = selectedIndex - 1;

        if (routerIndex >= static_cast<int>(history.size()))
            return;

        const auto& routerHistory = history[routerIndex];

        for (const auto& measurement : routerHistory)
        {
            pingValues.push_back(measurement.getPing());
            timeValues.push_back(measurement.getTimestamp());
        }

        m_pingChart->setTitle(
            QString("PING — ROUTER %1").arg(routerIndex + 1)
            );
    }

    // Передаём данные графику
    m_pingChart->setData(pingValues);
    m_pingChart->setTimeData(timeValues);
}
void MainWindow::updateRoutersTable()
{
    const auto& routers = m_networkManager.getRouters();
    const auto& measurements =
        m_networkManager.getLatestMeasurements();

    ui->routersTableWidget->setRowCount(
        static_cast<int>(routers.size())
        );

    for (std::size_t i = 0; i < routers.size(); i++)
    {
        const Router& router = routers[i];
        const Measurement& measurement = measurements[i];

        int row = static_cast<int>(i);

        // Определяем статус
        QString statusText;
        QColor statusColor;

        switch (router.getStatus())
        {
        case RouterStatus::OK:
            statusText = "OK";
            statusColor = QColor("#24B6A3");
            break;

        case RouterStatus::WARNING:
            statusText = "WARNING";
            statusColor = QColor("#EFCB45");
            break;

        case RouterStatus::CRITICAL:
            statusText = "CRITICAL";
            statusColor = QColor("#EC342C");
            break;
        }

        // Заполняем строку
        QStringList values = {
            QString::number(router.getId()),
            router.getName(),
            router.getLocation(),
            QString::number(measurement.getPing(), 'f', 1),
            QString::number(measurement.getJitter(), 'f', 1),
            QString::number(measurement.getpacketLoss(), 'f', 2),
            QString::number(measurement.getdownloadSpeed(), 'f', 1),
            QString::number(measurement.getuploadSpeed(), 'f', 1),
            statusText
        };

        for (int column = 0; column < values.size(); column++)
        {
            QTableWidgetItem* item =
                ui->routersTableWidget->item(row, column);

            if (item == nullptr)
            {
                item = new QTableWidgetItem();
                ui->routersTableWidget->setItem(
                    row, column, item
                    );
            }

            item->setText(values[column]);
            item->setTextAlignment(Qt::AlignCenter);

            // Цветовая индикация статуса
            if (column == 8)
            {
                item->setForeground(statusColor);
            }
        }
    }
}

void MainWindow::updateMap()
{
    // Проверяем готовность карты
    if (!m_mapReady || m_mapView == nullptr)
        return;

    // Получаем маршрутизаторы
    const auto& routers = m_networkManager.getRouters();

    // Создаём JSON-массив
    QJsonArray routersArray;

    for (const Router& router : routers)
    {
        // Определяем текстовый статус
        QString status;

        switch (router.getStatus())
        {
        case RouterStatus::OK:
            status = "OK";
            break;

        case RouterStatus::WARNING:
            status = "WARNING";
            break;

        case RouterStatus::CRITICAL:
            status = "CRITICAL";
            break;
        }

        // Формируем JSON-объект маршрутизатора
        QJsonObject routerObject;

        routerObject["id"] = router.getId();
        routerObject["name"] = router.getName();
        routerObject["location"] = router.getLocation();

        routerObject["latitude"] = router.getLatitude();
        routerObject["longitude"] = router.getLongitude();

        routerObject["status"] = status;

        // Добавляем объект в массив
        routersArray.append(routerObject);
    }

    // Преобразуем JSON в строку
    QString jsonData = QString::fromUtf8(
        QJsonDocument(routersArray).toJson(QJsonDocument::Compact)
        );

    // Вызываем JavaScript-функцию карты
    QString script = QString(
                         "window.updateRouters(%1);"
                         ).arg(jsonData);

    m_mapView->page()->runJavaScript(script);
}
void MainWindow::updateMapCards()
{
    const auto& routers = m_networkManager.getRouters();

    int totalRouters = 0;
    int onlineRouters = 0;
    int alerts = 0;

    for (const Router& router : routers)
    {
        totalRouters++;

        switch (router.getStatus())
        {
        case RouterStatus::OK:
            onlineRouters++;
            break;

        case RouterStatus::WARNING:
        case RouterStatus::CRITICAL:
            alerts++;
            break;
        }
    }

    ui->totalRoutersValue->setText(QString::number(totalRouters));
    ui->onlineRoutersValue->setText(QString::number(onlineRouters));
    ui->alertsValue->setText(QString::number(alerts));
}

void MainWindow::checkAlerts()
{
    const auto& routers = m_networkManager.getRouters();

    // Запоминаем количество событий до проверки
    const int previousCount = m_alertManager.getAlerts().size();

    // Проверяем состояние всех маршрутизаторов
    for (const Router& router : routers)
        m_alertManager.checkRouter(router);

    // Получаем обновлённую историю событий
    const auto& alerts = m_alertManager.getAlerts();

    // Если появились новые события — показываем уведомления
    for (int i = previousCount; i < alerts.size(); ++i)
    {
        showNotification(alerts[i]);
    }

    // Обновляем таблицу событий
    updateAlertsTable();
    updateReportsPage();
}

void MainWindow::updateAlertsTable()
{
    const QList<Alert>& alerts = m_alertManager.getAlerts();

    ui->alertsTableWidget->setRowCount(alerts.size());

    for (int i = 0; i < alerts.size(); ++i)
    {
        const Alert& alert = alerts[i];

        ui->alertsTableWidget->setItem(
            i, 0,
            new QTableWidgetItem(
                alert.getTimestamp().toString("dd.MM.yyyy HH:mm:ss")
                )
            );

        ui->alertsTableWidget->setItem(
            i, 1,
            new QTableWidgetItem(alert.getRouterName())
            );

        ui->alertsTableWidget->setItem(
            i, 2,
            new QTableWidgetItem(alert.getEvent())
            );

        QTableWidgetItem* severityItem =
            new QTableWidgetItem(alert.getSeverity());

        if (alert.getSeverity() == "CRITICAL")
        {
            severityItem->setForeground(QColor("#FF4D5E"));
        }
        else if (alert.getSeverity() == "WARNING")
        {
            severityItem->setForeground(QColor("#F5B942"));
        }
        else if (alert.getSeverity() == "INFO")
        {
            severityItem->setForeground(QColor("#35C98B"));
        }

        ui->alertsTableWidget->setItem(i, 3, severityItem);

        ui->alertsTableWidget->setItem(
            i, 4,
            new QTableWidgetItem(alert.getDescription())
            );

        // Настройка таблицы истории событий
        ui->reportsTableWidget->setColumnCount(5);

        ui->reportsTableWidget->setHorizontalHeaderLabels({
            "Time",
            "Router",
            "Event",
            "Severity",
            "Description"
        });

        // Запрещаем редактирование ячеек
        ui->reportsTableWidget->setEditTriggers(
            QAbstractItemView::NoEditTriggers
            );

        // Выделяем целые строки
        ui->reportsTableWidget->setSelectionBehavior(
            QAbstractItemView::SelectRows
            );

        // Разрешаем выделение только одной строки
        ui->reportsTableWidget->setSelectionMode(
            QAbstractItemView::SingleSelection
            );

        // Скрываем вертикальную нумерацию строк
        ui->reportsTableWidget->verticalHeader()->setVisible(false);

        // Убираем сетку
        ui->reportsTableWidget->setShowGrid(false);

        // Отключаем чередование цветов
        ui->reportsTableWidget->setAlternatingRowColors(false);

        // Настраиваем ширину столбцов
        ui->reportsTableWidget->setColumnWidth(0, 150);
        ui->reportsTableWidget->setColumnWidth(1, 130);
        ui->reportsTableWidget->setColumnWidth(2, 200);
        ui->reportsTableWidget->setColumnWidth(3, 110);

        // Последний столбец занимает оставшееся пространство
        ui->reportsTableWidget->horizontalHeader()->setStretchLastSection(true);


        ui->reportsTableWidget->setStyleSheet(R"(
    QTableWidget {
        background-color: #182230;
        alternate-background-color: #202C3B;
        color: #E5EAF0;
        border: 1px solid #303D4E;
        border-radius: 8px;
        gridline-color: transparent;
        font-size: 12px;
        selection-background-color: #30445C;
        selection-color: #FFFFFF;
        outline: none;
    }

    QHeaderView::section {
        background-color: #202C3B;
        color: #8B98A9;
        border: none;
        border-bottom: 1px solid #354355;
        padding: 12px 8px;
        font-size: 11px;
        font-weight: bold;
    }

    QTableWidget::item {
        padding: 9px 8px;
        border-bottom: 1px solid #293747;
    }

    QTableWidget::item:selected {
        background-color: #30445C;
        color: #FFFFFF;
    }

    QScrollBar:vertical {
        background-color: #182230;
        width: 8px;
        margin: 0;
    }

    QScrollBar::handle:vertical {
        background-color: #43546A;
        border-radius: 4px;
        min-height: 25px;
    }

    QScrollBar::add-line:vertical,
    QScrollBar::sub-line:vertical {
        height: 0;
    }
)");

    }
}

void MainWindow::showNotification(const Alert& alert)
{
    QString title;

    if (alert.getSeverity() == "CRITICAL")
        title = "CRITICAL FAILURE";
    else if (alert.getSeverity() == "WARNING")
        title = "NETWORK WARNING";
    else
        title = "ROUTER RECOVERED";

    QString message =
        alert.getRouterName() + " — " + alert.getDescription();

    // Создаём уведомление внутри главного окна
    auto* notification = new AlertNotification(
        title,
        message,
        alert.getSeverity(),
        this
        );
    // Максимальное количество одновременно отображаемых уведомлений
    const int MAX_NOTIFICATIONS = 3;

    // Удаляем самое старое уведомление, если достигнут лимит
    while (m_activeNotifications.size() >= MAX_NOTIFICATIONS)
    {
        if (!m_activeNotifications.isEmpty())
        {
            auto* oldest = m_activeNotifications.first().data();

            m_activeNotifications.removeFirst();

            if (oldest)
                oldest->deleteLater();
        }
    }
    // Добавляем его в список активных уведомлений
    m_activeNotifications.append(notification);

    // Удаляем указатель из списка после закрытия уведомления
    connect(notification, &QObject::destroyed, this, [this, notification]()
            {
                for (int i = m_activeNotifications.size() - 1; i >= 0; --i)
                {
                    if (m_activeNotifications[i].isNull())
                        m_activeNotifications.removeAt(i);
                }

                repositionNotifications();
            });

    notification->show();
    notification->raise();

    // Звуковое оповещение
    if (alert.getSeverity() == "WARNING")
    {
        QApplication::beep();
    }
    else if (alert.getSeverity() == "CRITICAL")
    {
        QApplication::beep();

        QTimer::singleShot(250, this, []()
                           {
                               QApplication::beep();
                           });
    }

    repositionNotifications();
}

void MainWindow::repositionNotifications()
{
    const int margin = 20;
    const int spacing = 12;

    int currentY = height() - margin;

    // Располагаем уведомления снизу вверх
    for (int i = m_activeNotifications.size() - 1; i >= 0; --i)
    {
        auto* notification = m_activeNotifications[i].data();

        if (!notification)
            continue;

        currentY -= notification->height();

        notification->move(
            width() - notification->width() - margin,
            currentY
            );

        notification->raise();

        currentY -= spacing;
    }
}

void MainWindow::updateReportsPage()
{
    const auto& alerts = m_alertManager.getAlerts();

    int warningCount = 0;
    int criticalCount = 0;

    // Пересчитываем количество событий
    for (const Alert& alert : alerts)
    {
        if (alert.getSeverity() == "WARNING")
            ++warningCount;
        else if (alert.getSeverity() == "CRITICAL")
            ++criticalCount;
    }

    // Обновляем карточки
    ui->totalEventsValue->setText(
        QString::number(alerts.size())
        );

    ui->warningsValue->setText(
        QString::number(warningCount)
        );

    ui->criticalEventsValue->setText(
        QString::number(criticalCount)
        );

    // Подготавливаем таблицу
    ui->reportsTableWidget->setRowCount(alerts.size());

    // Заполняем таблицу от новых событий к старым
    for (int i = 0; i < alerts.size(); ++i)
    {
        const Alert& alert = alerts[alerts.size() - 1 - i];

        // Время
        auto* timeItem = new QTableWidgetItem(
            alert.getTimestamp().toString("dd.MM.yyyy HH:mm:ss")
            );

        // Маршрутизатор
        auto* routerItem = new QTableWidgetItem(
            alert.getRouterName()
            );

        // Событие
        auto* eventItem = new QTableWidgetItem(
            alert.getEvent()
            );

        // Уровень важности
        auto* severityItem = new QTableWidgetItem(
            alert.getSeverity()
            );

        // Описание
        auto* descriptionItem = new QTableWidgetItem(
            alert.getDescription()
            );

        // Цвета в зависимости от важности события
        QColor severityColor;

        if (alert.getSeverity() == "CRITICAL")
            severityColor = QColor("#FF4D5E");
        else if (alert.getSeverity() == "WARNING")
            severityColor = QColor("#F5B942");
        else
            severityColor = QColor("#35C98B");

        severityItem->setForeground(severityColor);
        eventItem->setForeground(severityColor);

        // Выравнивание
        severityItem->setTextAlignment(Qt::AlignCenter);

        // Добавляем элементы в таблицу
        ui->reportsTableWidget->setItem(i, 0, timeItem);
        ui->reportsTableWidget->setItem(i, 1, routerItem);
        ui->reportsTableWidget->setItem(i, 2, eventItem);
        ui->reportsTableWidget->setItem(i, 3, severityItem);
        ui->reportsTableWidget->setItem(i, 4, descriptionItem);

        ui->reportsTableWidget->setRowHeight(i, 42);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
