#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <QThread>
#include <QSettings>
#include "ant.h"

class Widget;

class Movement : public QThread
{
    Q_OBJECT

public:
    explicit Movement(Widget *widget, QList<Ant*> &ants, int start, int stop, int interval, unsigned int seed);

protected:
    void run();
    void timerEvent(QTimerEvent *);

private:
    Widget *widget;
    QList<Ant*> *ants;
    QSettings *settings;
    int iStart;
    int iStop;
    int interval;
    unsigned int seed;

    void defaultSettings();
};

#endif // MOVEMENT_H
