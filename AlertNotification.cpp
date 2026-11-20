
    #include "AlertNotification.h"

    #include <QLabel>
    #include <QVBoxLayout>
    #include <QTimer>
    #include <QPushButton>
    #include <QHBoxLayout>

    AlertNotification::AlertNotification(
        const QString& title,
        const QString& message,
        const QString& severity,
        QWidget* parent
        )
        : QFrame(parent)//как то работает по страннм причинам
    {
        // Размер уведомления
        setFixedSize(300, 88);

        // Выбираем цвет в зависимости от серьёзности события
        QString accentColor;

        if (severity == "CRITICAL")
            accentColor = "#FF4D5E";
        else if (severity == "WARNING")
            accentColor = "#F5B942";
        else
            accentColor = "#35C98B";

        // Оформление самого уведомления
        setStyleSheet(QString(
                          "QFrame {" // задаем характеристики всплывающего окна
                          "background-color: #FFFFFF;"
                          "border: 1px solid %1;"
                          "border-left: 5px solid %1;"
                          "border-radius: 10px;"
                          "}"
                          "QLabel {"//характерестики надписей
                          "background: transparent;"
                          "border: none;"
                          "color: #282828;"
                          "}"
                          ).arg(accentColor));


        // Создаём вертикальный layout
        auto* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(12, 7, 12, 7);
        mainLayout->setSpacing(3);

        // Заголовок уведомления
        auto* titleLabel = new QLabel(title, this);

        titleLabel->setStyleSheet(
            QString(//характерестики надписи
                "color: %1;"
                "font-weight: bold;"
                "font-size: 12px;"
                ).arg(accentColor)
            );


        // Основной текст
        auto* messageLabel = new QLabel(message, this);
        messageLabel->setStyleSheet(
            "font-size: 10px;"
            );
        messageLabel->setWordWrap(true);

        // Верхняя строка с заголовком и кнопкой закрытия
        auto* headerLayout = new QHBoxLayout();


        auto* closeButton = new QPushButton("×", this);

        closeButton->setFixedSize(28, 28);
        closeButton->setCursor(Qt::PointingHandCursor);
        closeButton->setVisible(true);
        closeButton->setFlat(false);

        //ХАРАКТЕРИСТИКИ ЗАКРЫВАЮЩЕЙ КНОПКИ
        closeButton->setStyleSheet(R"(
            QPushButton {
                color: #777777;
                background-color: #F2F4F6;
                border: 1px solid #DDE2E7;
                border-radius: 6px;
                font-size: 20px;
                font-weight: bold;
                padding: 0px;
            }

            QPushButton:hover {
                color: #FFFFFF;
                background-color: #C82058;
                border: 1px solid #C82058;
            }

            QPushButton:pressed {
                background-color: #A81749;
            }
        )");


        closeButton->setStyleSheet(
            "QPushButton {"
            "color: #AAB4C3;"
            "border: none;"
            "font-size: 18px;"
            "}"
            "QPushButton:hover {"
            "color: ##8C4242;"
            "}"
            );

        headerLayout->addWidget(titleLabel);
        headerLayout->addStretch();
        headerLayout->addWidget(closeButton);

        mainLayout->addLayout(headerLayout);
        mainLayout->addWidget(messageLabel);

        // Ручное закрытие уведомления
        connect(closeButton, &QPushButton::clicked,
                this, &QWidget::deleteLater);

        // Автоматическое закрытие через 6 секунд
        QTimer::singleShot(4500, this, &QWidget::deleteLater);
    }
