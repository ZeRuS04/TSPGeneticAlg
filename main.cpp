#include <QGUIApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QDebug>
#include "Adapter.h"
#include <time.h>

int main(int argc, char *argv[])
{
    qsrand ( time(NULL) );
    QGuiApplication app(argc, argv);
    Adapter adapter;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("Adapter", &adapter);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    for(int i = 50; i <= 2000; i+=50) {
        adapter.start(i, 0.5, 0.001, 20000);
    }
    return app.exec();
}

