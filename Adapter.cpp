#include "Adapter.h"
#include <QDebug>
#include "QFile"

Adapter::Adapter(QObject *parent)
    : QObject(parent)
    , m_result(DBL_MAX)
{
    GeneticAlgorithm gAlg(0,0,0,0);
    readCoordinates();
    gAlg.setCoordinates(m_coordinates);
    setTowns(gAlg.coordinates());
    readOptimalRout();
//    connect(&gAlg, &GeneticAlgorithm::updateRoute, this, &Adapter::updateRoute);
//    gAlg.start();
}

Adapter::~Adapter()
{
//    m_gAlg.exit(1);
}

void Adapter::setTowns(QVector<QPointF> coord)
{
    foreach(QPointF point, coord) {
        m_towns << QVariant(point);
    }
    emit townsChanged(m_towns);
}

void Adapter::updateRoute(QList<int> *route, double result)
{
    if(result >= m_result) {
        delete route;
        return;
    }
    m_result = result;
    m_resultRoute.clear();
    foreach(int i, *route){
        m_resultRoute << QVariant(i);
    }
    delete route;
    emit resultRouteChanged(m_resultRoute);
}

QVector<QPointF> Adapter::coordinates() const
{
    return m_coordinates;
}

void Adapter::setCoordinates(const QVector<QPointF> &coordinates)
{
    m_coordinates = coordinates;
}

Adapter::start(int power, double pCross, double pMutate, int genCount)
{
    GeneticAlgorithm *ga = new GeneticAlgorithm(power, pCross, pMutate, genCount);
    ga->setCoordinates(m_coordinates);
    connect(ga, &GeneticAlgorithm::updateRoute, this, &Adapter::updateRoute);
    ga->start();
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

    QPointF firstPoint;
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
            oldPoint = firstPoint = point;
            continue;
        }
        QLineF lineF(oldPoint,point);
        result += lineF.length();
        oldPoint = point;
    }

    m_optimalRoute << m_optimalRoute.first();
    QLineF lineF(oldPoint,firstPoint);
    result += lineF.length();

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

