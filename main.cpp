#include <QGUIApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include "Adapter.h"
#include <time.h>

int main(int argc, char *argv[])
{
    qsrand ( time(NULL) );
    QGuiApplication app(argc, argv);
    Adapter adapter;

    for(int i = 50; i <= 2000; i+=50) {
        GeneticAlgorithm ga(i, 0.5, 0.001, 10000);
        ga.setCoordinates(adapter.coordinates());
        ga.run();
    }
//    QQmlApplicationEngine engine;
//    engine.rootContext()->setContextProperty("Adapter", &adapter);
//    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}

