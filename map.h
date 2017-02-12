#ifndef MAP_H
#define MAP_H

#include <QWidget>
#include <QMutex>
#include <QImage>
#include "ant.h"

class Map : public QWidget
{
    Q_OBJECT

public:
    enum State { Running, Paused, WaitingFood, WaitingAnthill };
    enum Data { Center, Left, LeftUp, Up, RightUp, Right, RightDown, Down, LeftDown };

    explicit Map(int nAnts, int width, int height, const QPoint &foodPos, const QPoint &anthillPos, QWidget *parent = 0);
    ~Map();

    bool insideMap(const QPoint &point);
    void leavePheromone(const QPoint &point, float quantity);
    void getPheromones(const QPoint &point, float data[]);
    void updatePheromone(float factor);

public slots:
    void runSimulation();
    void pauseSimulation();
    void waitForFood();
    void waitForAnthill();

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent *);

private:
    State state;
    int map_width;
    int map_height;
    QVector<float> pheromones;
    QImage *image;
    QList<Ant*> ants;
    QPoint foodPosition;
    QPoint anthillPosition;
    QMutex mutex;
};

#endif // MAP_H
