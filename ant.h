#ifndef ANT_H
#define ANT_H

#include <QThread>
#include <QPoint>
#include <QMutex>

class Map;

class Ant
{
public:
    explicit Ant(const QPoint pos) : old_dx(0), old_dy(0), position(pos) { }

    inline QPoint getPosition()
    {
        QPoint point;
        mutex.lock();
        point = position;
        mutex.unlock();
        return point;
    }

    inline void setPosition(const QPoint &point)
    {
        mutex.lock();
        position = point;
        mutex.unlock();
    }

    int old_dx;
    int old_dy;

private:
    QPoint position;
    QMutex mutex;
};

#endif // ANT_H
