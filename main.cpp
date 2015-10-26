#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Adapter.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterUncreatableType<Adapter>("Adapter", 1, 0, "Adapter","");
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

