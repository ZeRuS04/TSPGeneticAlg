#ifndef ADAPTER_H
#define ADAPTER_H

#include <QObject>
#include <GeneticAlgorithm.h>

class Adapter : public QObject
{
    Q_OBJECT
public:
    explicit Adapter(QObject *parent = 0);

signals:

public slots:

private:
    GeneticAlgorithm m_gAlg;
};

#endif // ADAPTER_H
