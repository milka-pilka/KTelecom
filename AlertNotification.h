
#ifndef ALERTNOTIFICATION_H
#define ALERTNOTIFICATION_H

#include <QFrame>
#include <QString>

class AlertNotification : public QFrame
{
public:
    explicit AlertNotification(
        const QString& title,
        const QString& message,
        const QString& severity,
        QWidget* parent = nullptr
        );
};

#endif // ALERTNOTIFICATION_H
