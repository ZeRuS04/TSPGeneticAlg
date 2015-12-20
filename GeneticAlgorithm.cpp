#include "GeneticAlgorithm.h"
#include <QDebug>
#include <QFile>
#include <QLineF>
#include <QRegExp>
#include <time.h>

double fRand(double fMin, double fMax)
{
    double f = (double)qrand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

GeneticAlgorithm::GeneticAlgorithm(int power, double pCross, double pMutate, int genCount)
    : m_coordCount(0)
    , m_power(power)
    , m_pCross(pCross)
    , m_pMutate(pMutate)
    , m_genCount(genCount)
    , m_crossingType(HUERISTIC_CROSSOVER)
    , m_selectionType(TOURNEY)
{
    setStackSize(2);
}

void GeneticAlgorithm::fitnesFunction(gene* gene)
{
    gene->fitness = 0;
    ushort currentTown = gene->alleles[0], nextTown;
    QVector<ushort> visitedTowns;
    visitedTowns << currentTown;

    for(int i = 0; i < gene->length-1; i++) {
        nextTown = gene->alleles[currentTown];
        if(visitedTowns.contains(nextTown) || currentTown == nextTown) {
            gene->fitness = DBL_MAX;
            return;
        }
        visitedTowns << nextTown;
        QLineF line(m_coordinates.at(currentTown),m_coordinates.at(nextTown));
        gene->fitness += line.length();
        currentTown = nextTown;
    }

    QLineF line(m_coordinates.at(currentTown),m_coordinates.at(visitedTowns.first()));
    gene->fitness += line.length();
    return;
}

gene GeneticAlgorithm::crossOver(gene* parent1, gene* parent2)
{
    gene g;
    g.alleles = new unsigned short[m_coordCount];
    g.length = m_coordCount;
    switch(m_crossingType) {
    case HUERISTIC_CROSSOVER: {
        ushort currentTown = qrand()%(m_coordCount-1);   //выбираем случайный начальный город
        ushort nextTown_1, nextTown_2;

        QVector<ushort> visitedTowns;
        visitedTowns << currentTown;                    // Заносим город в посещенные

        for(int i = 0; i < m_coordCount; i++) {         // Для каждого гена
            //Возьмем путь из текущего города для каждого из родителей
            nextTown_1 = parent1->alleles[currentTown];
            nextTown_2 = parent2->alleles[currentTown];

            // Закончим цикл если это последняя итерация
            if( i == m_coordCount-1) {
                g.alleles[currentTown] = visitedTowns.at(0);
                continue;
            }

            // Если оба варианта ведут к преждевременному циклу
            if( visitedTowns.contains(nextTown_1) && visitedTowns.contains(nextTown_2) ){
                ushort n;
                // сгенерируем е любой еще не посещенный город:
                if(visitedTowns.length() >= m_coordCount-1) {
                    n = 0;
                    while(visitedTowns.contains(n)){
                        n++;
                    }
                } else {
                    do {
                        n = qrand()%(m_coordCount-1);
                    } while(visitedTowns.contains(n));
                }
                g.alleles[currentTown] = n; // и посетим его следующим
                currentTown = n;
                visitedTowns << currentTown;                    // Заносим город в посещенные
                continue;
            }

            // Подошел только один из городов:
            if( !visitedTowns.contains(nextTown_1) && visitedTowns.contains(nextTown_2) ){
                g.alleles[currentTown] = nextTown_1;
                currentTown = nextTown_1;
                visitedTowns << currentTown;                    // Заносим город в посещенные
                continue;
            }
            if( visitedTowns.contains(nextTown_1) && !visitedTowns.contains(nextTown_2) ){
                g.alleles[currentTown] = nextTown_2;
                currentTown = nextTown_2;
                visitedTowns << currentTown;                    // Заносим город в посещенные
                continue;
            }

            // Если подходят оба города:
            if( !visitedTowns.contains(nextTown_1) && !visitedTowns.contains(nextTown_2) ){
                // Сравниваем расстояния до городов и выбираем наименьшее
                QLineF line_1(m_coordinates.at(currentTown), m_coordinates.at(nextTown_1));
                QLineF line_2(m_coordinates.at(currentTown), m_coordinates.at(nextTown_2));
                double l1 = line_1.length();
                double l2 = line_2.length();
                if(l1 < l2) {
                   g.alleles[currentTown] = nextTown_1;
                   currentTown = nextTown_1;
                } else {
                    g.alleles[currentTown] = nextTown_2;
                    currentTown = nextTown_2;
                }
                visitedTowns << currentTown;                    // Заносим город в посещенные
                continue;
            }
        }
        break;
    }
    }
    fitnesFunction(&g);
    return g;
}

void GeneticAlgorithm::initGenerator()
{
    m_genotype.clear();
    for(int i = 0; i <  m_power; i++) {
        gene g;
        g.alleles = new unsigned short[m_coordCount];
        g.length = m_coordCount;
        QVector<ushort> usedNums;
        for (int i = 0; i < m_coordCount; i++) {
            ushort n;
            do{
                n = qrand()%(m_coordCount);
            } while(usedNums.contains(n));
            usedNums << n;
            g.alleles[i] = n;
        }
        fitnesFunction(&g);
        if(g.fitness == DBL_MAX)
            i--;
        else
            m_genotype << g;
    }
}

void GeneticAlgorithm::selection()
{
    int parentsCount = 0;
    gene parentsArray[2];
    m_newGens.clear();

    switch (m_selectionType) {
    case ROULETTE_WHEEL:{
        qSort(m_genotype);
        double *wheel = new double[m_power];
        wheel[0] = 1/m_genotype.at(0).fitness;    //Значение ФитнессФункции для 1-ого генома
        for (int i = 1; i < m_power; i++){
            wheel[i] = wheel[i-1] + 1/m_genotype.at(i).fitness;   //Значение ФитнессФункции для i-ого генома
        }
        double all = wheel[m_power-1];

        for (int i = 0; i < m_power; i++){
            double chance = fRand(0,1);
            if(chance > m_pCross)
                continue;

            double index = fRand(0,1) * all;
            int l = 0;
            int r = m_power-1;
            int c = 0;
            while (l < r){
                if(r - l == 1)
                    break;
                c = (l+r) >> 1;
                if (wheel[c] < index)
                    l = c;
                else
                    r = c;
            }

            parentsArray[parentsCount] = m_genotype.at(l);
            parentsCount++;
            if(parentsCount == 2) {
                m_newGens << crossOver(&parentsArray[0],&parentsArray[1]);
                parentsCount = 0;
            }
        }
    }break;
    case TOURNEY: {
        QVector<gene> genotype = m_genotype;
        while (genotype.length() >= 2) {
            int index1 = qrand()%(genotype.length() - 1);
            gene g1 = genotype.at(index1);
            genotype.remove(index1);
            int index2;
            if(genotype.length() == 1)
                index2 = 0;
            else
                index2 = qrand()%(genotype.length() - 1);
            gene g2 = genotype.at(index2);
            genotype.remove(index2);

            double fr1 = g1.fitness; //Значение ФитнессФункции для index1 Генома
            double fr2 = g2.fitness; //Значение ФитнессФункции для index2 Генома

            parentsArray[parentsCount] = fr1 < fr2 ? g1
                                                   : g2;
            parentsCount++;

            if(parentsCount == 2) {
                m_newGens << crossOver(&parentsArray[0],&parentsArray[1]);
                parentsCount = 0;
            }
        }
    } break;
    }
}

void GeneticAlgorithm::reductionOperator()
{
    m_genotype << m_newGens;
    qSort(m_genotype);
    while(m_genotype.length() > m_power) {
        delete m_genotype.last().alleles;
        m_genotype.removeLast();
    }
}

void GeneticAlgorithm::mutationOperator()
{
    for(int i = 0; i < m_newGens.length(); i++) {
        double chance = fRand(0,1);
        if(chance <= m_pMutate){
            ushort n1 = qrand()%(m_coordCount - 1);
            ushort n2 = qrand()%(m_coordCount - 1);
            ushort tmp = m_newGens[i].alleles[n1];
            m_newGens[i].alleles[n1] = m_newGens[i].alleles[n2];
            m_newGens[i].alleles[n2] = tmp;
            fitnesFunction(&m_newGens[i]);
        }
    }
}

void GeneticAlgorithm::run()
{

    setPriority(QThread::LowestPriority);
    qsrand ( time(NULL) );
    initGenerator();
    int i;
    for (i = 0; i < m_genCount; i++) {

        qsrand ( time(NULL) );
        selection();
        mutationOperator();
        reductionOperator();


        QVector<gene> tmp = m_genotype;
        tmp.removeAll(tmp.first());
        if (tmp.isEmpty()) {
            break;
        }

    }
    QList<int> *route = new QList<int>;
    qDebug() << m_genotype.first().fitness << "\t" << i;
    int currentTown = 0;
    route->append(currentTown);
    for(int i = 0; i < m_genotype.first().length; i++)
    {
        int nextTown = m_genotype.first().alleles[currentTown];
        route->append(nextTown);
        currentTown = nextTown;
    }

    emit updateRoute(route, m_genotype.first().fitness);
}

int GeneticAlgorithm::genCount() const
{
    return m_genCount;
}

void GeneticAlgorithm::setGenCount(int genCount)
{
    m_genCount = genCount;
}

double GeneticAlgorithm::pMutate() const
{
    return m_pMutate;
}

void GeneticAlgorithm::setPMutate(double pMutate)
{
    m_pMutate = pMutate;
}

double GeneticAlgorithm::pCross() const
{
    return m_pCross;
}

void GeneticAlgorithm::setPCross(double pCross)
{
    m_pCross = pCross;
}

int GeneticAlgorithm::power() const
{
    return m_power;
}

void GeneticAlgorithm::setPower(int power)
{
    m_power = power;
}

QVector<gene> GeneticAlgorithm::genotype() const
{
    return m_genotype;
}

void GeneticAlgorithm::setGenotype(const QVector<gene> &genotype)
{
    m_genotype = genotype;
}

QVector<QPointF> GeneticAlgorithm::coordinates() const
{
    return m_coordinates;
}

void GeneticAlgorithm::setCoordinates(const QVector<QPointF> &coordinates)
{
    m_coordinates = coordinates;
    m_coordCount = coordinates.length();
}
