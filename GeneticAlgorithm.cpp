#include "GeneticAlgorithm.h"
#include <QDebug>
#include <QFile>
#include <QLineF>
#include <QRegExp>

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

GeneticAlgorithm::GeneticAlgorithm()
    : m_coordCount(0)
    , isOk(false)
    , m_crossingType(HUERISTIC_CROSSOVER)
    , m_selectionType(ROULETTE_WHEEL)
{
    isOk = readCoordinates();
}

bool GeneticAlgorithm::readCoordinates()
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
        m_coordCount++;
    }
    if(m_coordinates.isEmpty())
        return false;
    else
        return true;
}

void GeneticAlgorithm::fitnesFunction(gene* gene)
{
    gene->fitness = 0;
    ushort currentTown = 0, nextTown;
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
}

gene GeneticAlgorithm::crossOver(gene* parent1, gene* parent2)
{
    gene g;
    g.alleles = new unsigned short[m_coordCount];
    g.length = m_coordCount;
    switch(m_crossingType) {
    case HUERISTIC_CROSSOVER:
        ushort currentTown = rand()%(m_coordCount-1);   //выбираем случайный начальный город
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
                ushort n = rand()%(m_coordCount-1);
                // сгенерируем е любой еще не посещенный город:
                while(visitedTowns.contains(n)){
                    n++;
                    if(n >= m_coordCount)
                        n = 0;
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
                QLineF line_1(m_coordinates.at(currentTown), m_coordinates.at(parent1->alleles[currentTown]));
                QLineF line_2(m_coordinates.at(currentTown), m_coordinates.at(parent2->alleles[currentTown]));
                if(line_1.length() > line_2.length()) {
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
        fitnesFunction(&g);
        return g;
        break;
    }
}

void GeneticAlgorithm::initGenerator()
{
    m_genotype.clear();
    for(int i = 0; i <  GA_POWER; i++) {
        gene g;
        g.alleles = new unsigned short[m_coordCount];
        g.length = m_coordCount;
        QVector<ushort> usedNums;
        for (int i = 0; i < m_coordCount; i++) {
            ushort n;
            do{
                n = rand()%(m_coordCount);
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
        double *wheel = new double[GA_POWER];
        wheel[0] = 1/m_genotype.at(0).fitness;    //Значение ФитнессФункции для 1-ого генома
        for (int i = 1; i < GA_POWER; i++){
            wheel[i] = wheel[i-1] + 1/m_genotype.at(i).fitness;   //Значение ФитнессФункции для i-ого генома
        }
        double all = wheel[GA_POWER-1];

        for (int i = 0; i < GA_POWER; i++){
            double chance = fRand(0,1);
            if(chance > GA_P_CROSS)
                continue;

            double index = fRand(0,1) * all;
            int l = 0;
            int r = GA_POWER-1;
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
        for (int i = 0; i < GA_POWER; i++){
            double chance = fRand(0,1);
            if(chance > GA_P_CROSS)
                continue;
            int index1 = rand()%GA_POWER - 1;
            int index2 = rand()%GA_POWER - 1;;

            double fr1 = m_genotype.at(index1).fitness; //Значение ФитнессФункции для index1 Генома
            double fr2 = m_genotype.at(index2).fitness; //Значение ФитнессФункции для index2 Генома

            parentsArray[parentsCount] = fr1 > fr2 ? m_genotype.at(index1)
                                                   : m_genotype.at(index2);
            parentsCount++;

            if(parentsCount == 2) {
                m_newGens << crossOver(&parentsArray[0],&parentsArray[1]);
                parentsCount = 0;
            }
        }
    }break;
    }
}

void GeneticAlgorithm::reductionOperator()
{
    m_genotype << m_newGens;
    qSort(m_genotype);
    if(m_genotype.length() > GA_POWER){
        m_genotype.remove(GA_POWER, m_genotype.length() - GA_POWER);
    }
}

void GeneticAlgorithm::mutationOperator()
{
    for(int i = 0; i < m_newGens.length(); i++) {
        double chance = fRand(0,1);
        if(chance <= GA_P_MUTATE){
            ushort n1 = rand()%(m_coordCount);
            ushort n2 = rand()%(m_coordCount);
            ushort tmp = m_newGens[i].alleles[n1];
            m_newGens[i].alleles[n1] = m_newGens[i].alleles[n2];
            m_newGens[i].alleles[n2] = tmp;
            fitnesFunction(&m_newGens[i]);
            qDebug() << "Mutation!";
        }
    }
}

void GeneticAlgorithm::run()
{
    if(!isOk) {
        qWarning("Error. Coordinates was't initialized.");
        return;
    }
    initGenerator();

    for (int i = 0; i < GA_GENERATION_COUNT; i++) {

//        QList<double> *points = new QList<double>();
//        for( int j = 0; j < GA_POWER; j++){
//            points->append(m_genotype.at(j).x);
//        }
//        emit updatePoints(points);
        selection();
        mutationOperator();
        reductionOperator();


//        QVector<gene> tmp = m_genotype;
//        tmp.removeAll(tmp.first());
//        if (tmp.isEmpty()) {
//            qDebug() << "EARLY END";
//            qDebug() << "Total iterations: " << i;
//            break;
//        }
    }

    QList<int> *route = new QList<int>;
    qDebug() << "Result:" << m_genotype.first().fitness << endl;
    int currentTown = 0;
    route->append(currentTown);
    for(int i = 0; i < m_genotype.first().length; i++)
    {
        int nextTown = m_genotype.first().alleles[currentTown];
        route->append(nextTown);
        qDebug() << m_genotype.first().alleles[currentTown]+1;
        currentTown = nextTown;
    }

    emit updateRoute(route);
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
}
