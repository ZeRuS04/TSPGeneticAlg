#include <QGUIApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include "Adapter.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    Adapter adapter;
//    qmlRegisterUncreatableType<Adapter>("Adapter", 1, 0, "Adapter","");
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("Adapter", &adapter);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}

