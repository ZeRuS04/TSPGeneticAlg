#include "Adapter.h"
#include "QFile"

Adapter::Adapter(QObject *parent) : QObject(parent)
{
    setTowns(m_gAlg.coordinates());
    readCoordinates();
    connect(&m_gAlg, &GeneticAlgorithm::updateRoute, this, &Adapter::updateRoute);
    m_gAlg.start();
}

Adapter::~Adapter()
{
    m_gAlg.exit(1);
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

bool Adapter::readCoordinates()
{
    QFile file("c:/optimal.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    while (!file.atEnd()) {
        QString line(file.readLine());

        m_optimalRoute << QVariant(line.toInt()-1);
    }
    emit optimalRouteChanged();
    if(m_optimalRoute.isEmpty())
        return false;
    else
        return true;
}

