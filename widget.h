#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMutex>
#include <QSettings>
#include <QImage>
#include "movement.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    enum Data { Center, Left, LeftUp, Up, RightUp, Right, RightDown, Down, LeftDown };

    explicit Widget(QWidget *parent = 0);
    ~Widget();

    bool insideMap(const QPoint &point);
    void leavePheromone(const QPoint &point);
    void getPheromones(const QPoint &point, float data[]);

protected:
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent * event);
    void showEvent(QShowEvent *);

private:
    int timerUpdating;
    int timerDegrade;
    QMutex mutex;
    QVector<float> pheromones;
    QList<Ant*> ants;
    QList<Movement*> threads;
    QImage *image;
    QPoint anthillPosition;
    QPoint foodPosition;
    QSettings *settings;

    void degradePheromone();
    void defaultSettings();
};

#endif // WIDGET_H
