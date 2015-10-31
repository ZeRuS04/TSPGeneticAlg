#ifndef ADAPTER_H
#define ADAPTER_H

#include <QObject>
#include <QVariantList>
#include <QVariant>
#include "GeneticAlgorithm.h"

class Adapter : public QObject
{
    Q_OBJECT

public:
    explicit Adapter(QObject *parent = 0);
    ~Adapter();

    Q_PROPERTY(QVariantList towns READ towns NOTIFY townsChanged)

//    Q_PROPERTY(QList<int> optimalRoute READ optimalRoute NOTIFY optimalRouteChanged)
    Q_PROPERTY(QVariantList resultRoute READ resultRoute NOTIFY resultRouteChanged)


    QVariantList towns() const;

//    QList<int> optimalRoute() const;

    QVariantList resultRoute() const
    {
        return m_resultRoute;
    }

signals:

    void townsChanged(QVariantList arg);

    void optimalRouteChanged();

    void resultRouteChanged(QVariantList arg);

public slots:
    bool readCoordinates();
    void setTowns(QVector<QPointF> coord);
    void updateRoute(QList<int> *route);

private:
    GeneticAlgorithm m_gAlg;
    QVariantList m_towns;
    QList<int> m_optimalRoute;
    QVariantList m_resultRoute;
};

#endif // ADAPTER_H
