#include <QGUIApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include "Adapter.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    qsrand ( time(NULL) );
    QGuiApplication app(argc, argv);
    Adapter adapter;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("Adapter", &adapter);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}

