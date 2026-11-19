#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "NetworkManager.h"
#include "ScenarioController.h"
#include "TelemetryGenerator.h"
#include "GarchProcess.h"
#include "Router.h"
#include "Measurement.h"
#include "NetworkAnalyzer.h"
#include <QDebug>

int main(int argc, char *argv[])
{
    //Показуха для роутера
    /*
    Router router(
        1,
        "Router 1",
        "Екатеринбург",
        56.8386,
        60.6055,
        RouterStatus::OK
    );

    qDebug() << "Router ID:" << router.getId();
    qDebug() << "Name:" << router.getName();
    qDebug() << "Location:" << router.getLocation();
    qDebug() << "Latitude:" << router.getLatitude();
    qDebug() << "Longitude:" << router.getLongitude();
    */

    //Показуха для сети
    /*
    Measurement measurement(
        1,
        35.5,
        85.2,
        42.1,
        2.5,
        4.0,
        QDateTime::currentDateTime()
        );
    RouterStatus status = NetworkAnalyzer::analyze(measurement);

    if (status == RouterStatus::OK)
    {
        qDebug() << "Network status: OK";
    }
    else if (status == RouterStatus::WARNING)
    {
        qDebug() << "Network status: WARNING";
    }
    else
    {
        qDebug() << "Network status: CRITICAL";
    }
    qDebug() << "Router ID:" << measurement.getrouterId();
    qDebug() << "Ping:" << measurement.getPing() << "ms";
    qDebug() << "Download:" << measurement.getdownloadSpeed() << "Mbps";
    qDebug() << "Upload:" << measurement.getuploadSpeed() << "Mbps";
    qDebug() << "Packet Loss:" << measurement.getpacketLoss() << "%";
    qDebug() << "Jitter:" << measurement.getJitter() << "ms";
    qDebug() << "Timestamp:" << measurement.getTimestamp().toString(Qt::ISODate);
    */

    //Показуха для генератора сети
    /*
    TelemetryGenerator generator(
        1,
        35.0,
        3.0,
        100.0,
        50.0,
        42
        );

    NetworkManager network;

    for (int step = 0; step < 5; step++)
    {
        network.update();

        qDebug() << "\n===== STEP:" << step << "=====";

        const auto& routers = network.getRouters();

        const auto& measurements =
            network.getLatestMeasurements();

        for (std::size_t i = 0; i < routers.size(); i++)
        {
            qDebug() << "Router:"
                     << routers[i].getName();

            qDebug() << "Ping:"
                     << measurements[i].getPing();

            qDebug() << "Status:"
                     << static_cast<int>(
                            routers[i].getStatus()
                            );

            qDebug() << "----------------";
        }
    }
    */

    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "KTelecom1_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return QApplication::exec();
}
