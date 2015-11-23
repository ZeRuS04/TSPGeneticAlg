#include "Adapter.h"
#include <QDebug>
#include "QFile"

Adapter::Adapter(QObject *parent) : QObject(parent)
{
    readCoordinates();
    m_gAlg.setCoordinates(m_coordinates);
    setTowns(m_gAlg.coordinates());
    readOptimalRout();
    connect(&m_gAlg, &GeneticAlgorithm::updateRoute, this, &Adapter::updateRoute);
    m_gAlg.run();
}

Adapter::~Adapter()
{
}

void Adapter::setTowns(QVector<QPointF> coord)
{
    foreach(QPointF point, coord) {
        m_towns << QVariant(point);
    }
    emit townsChanged(m_towns);
}

void Adapter::updateRoute(QList<int> *route)
{
    foreach(int i, *route){
        m_resultRoute << QVariant(i);
    }
    emit resultRouteChanged(m_resultRoute);
}

QVariantList Adapter::towns() const
{
    return m_towns;
}

QVariantList Adapter::optimalRoute() const
{
    return m_optimalRoute;
}

bool Adapter::readOptimalRout()
{
    QFile file("c:/optimal.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QPointF oldPoint;
    qreal result = 0;
    bool first = true;

    while (!file.atEnd()) {
        QString line(file.readLine());
        int n = line.toInt()-1;
        m_optimalRoute << QVariant(n);

        QPointF point = m_coordinates.at(n);

        if (first) {
            first = false;
            oldPoint = point;
            continue;
        }
        QLineF lineF(oldPoint,point);
        result += lineF.length();
        oldPoint = point;
    }
    emit optimalRouteChanged();
    m_optimalPath = result;
    qDebug() << "Optimal route result: " << result;
    if(m_optimalRoute.isEmpty())
        return false;
    else
        return true;
}

bool Adapter::readCoordinates()
{
    QFile file(FILE_STRING);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    while (!file.atEnd()) {
        QString line(file.readLine());
        QStringList list = line.split(" ");

        bool ok;
        double x = list.at(1).toDouble(&ok);
        if(!ok)
            continue;

        double y = list.at(2).toDouble(&ok);

        if(!ok)
            continue;
        QPointF point(x,y);
        m_coordinates << point;
    }
    return true;
}

