#ifndef GENETICALGORITHM_H
#define GENETICALGORITHM_H

#include <QThread>
#include <QVector>
#include <QList>
#include <QPointF>
#include <float.h>
#include <QtConcurrent>
#include <qmath.h>

#define GA_POWER        50
#define GA_P_CROSS      0.5
#define GA_P_MUTATE     0.001
#define GA_GENERATION_COUNT      100

#define FILE_STRING     "c:/coord.txt"
enum SelectionType {
    TOURNEY,
    ROULETTE_WHEEL
};

enum CrossingType {
    HUERISTIC_CROSSOVER
};

struct gene {
    unsigned short *alleles;
    unsigned short length;
    double fitness;

    bool operator<(gene const & b) const {return ((this->fitness) < (b.fitness));}
    // Test for equality.
//    bool operator==(gene const & gn) const  {return ((this->alleles & GA_MASK) == (gn.alleles & GA_MASK));}
};

struct parents {
    gene parents[2];
    char count;
};

class  GeneticAlgorithm : public QObject
{
    Q_OBJECT

    void fitnesFunction(gene* gene);
    void crossOver(parents parentsArray);
public:
    explicit GeneticAlgorithm();

    void initGenerator();
    void selection();
    void reductionOperator();
    void mutationOperator();
    QVector<QPointF> coordinates() const;
    void setCoordinates(const QVector<QPointF> &coordinates);

    QVector<gene> genotype() const;
    void setGenotype(const QVector<gene> &genotype);
    void run();

signals:
    void updateRoute(QList<int> *route);

public slots:

private:
    QVector<gene> m_genotype;
    QVector<gene> m_newGens;
    QVector<parents> m_famaly;

    QVector<QPointF> m_coordinates;
    ushort m_coordCount;

    SelectionType m_selectionType;
    CrossingType m_crossingType;
    bool isOk;
};

#endif // GENETICALGORITHM_H
