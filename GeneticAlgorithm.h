#ifndef GENETICALGORITHM_H
#define GENETICALGORITHM_H

#include <QThread>
#include <QVector>
#include <QList>
#include <QPointF>
#include <float.h>
#include <qmath.h>

#define GA_POWER        450
#define GA_P_CROSS      0.5
#define GA_P_MUTATE     0.001
#define GA_GENERATION_COUNT      1000

#define FILE_STRING     "c:/coord.txt"
enum SelectionType {
    TOURNEY,
    ROULETTE_WHEEL
};

enum CrossingType {
    HUERISTIC_CROSSOVER,
    ALTERNATING_EDGES
};

struct gene {
    unsigned short *alleles;
    unsigned short length;
    double fitness;

    bool operator<(gene const & b) const {return ((this->fitness) < (b.fitness));}
    // Test for equality.
    bool operator==(gene const & gn) const  {
        if(this->fitness != gn.fitness)
            return false;

        if(this->length != gn.length)
            return false;

        for(int i = 0; i < this->length; i++) {
            if(this->alleles[i] != gn.alleles[i])
                return false;
        }
        return true;
    }
};

class  GeneticAlgorithm : public QThread
{
    Q_OBJECT

    void fitnesFunction(gene* gene);
    gene crossOver(gene* parent1, gene* parent2);
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

protected:
    // QThread interface
    void run();

signals:
    void updateRoute(QList<int> *route);

public slots:

private:
    QVector<gene> m_genotype;
    QVector<gene> m_newGens;

    QVector<QPointF> m_coordinates;
    ushort m_coordCount;

    SelectionType m_selectionType;
    CrossingType m_crossingType;
    bool isOk;
};

#endif // GENETICALGORITHM_H
