#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "NetworkManager.h"
#include <QMainWindow>
#include <QWebEngineView>
#include "AlertManager.h"
#include "AlertNotification.h"
#include <QPointer>
#include "Alert.h"


class NetworkChart;
class QTimer;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
private:
    NetworkManager m_networkManager;
    QTimer* m_updateTimer = nullptr;
    NetworkChart* m_pingChart = nullptr;
    QWebEngineView* m_mapView = nullptr;
    bool m_mapReady = false;
    AlertManager m_alertManager;
    QList<QPointer<AlertNotification>> m_activeNotifications;


    void updateDashboard();
    void updateRoutersTable();
    void updateAnalyticsChart();
    void updateMap();
    void updateMapCards();
    void checkAlerts();
    void updateAlertsTable();
    void showNotification(const Alert& alert);
    void repositionNotifications();
    void updateReportsPage();
};
#endif // MAINWINDOW_H
