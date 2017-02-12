#include "movement.h"
#include "widget.h"

static const char settingsPath[] = "Behaviour.ini";

Movement::Movement(Widget *widget, QList<Ant*> &ants, int start, int stop, int interval, unsigned int seed) : QThread(NULL)
{
    this->widget = widget;
    this->ants = &ants;
    settings = new QSettings(settingsPath, QSettings::IniFormat, this);
    iStart = start;
    iStop = stop;
    this->interval = interval;
    this->seed = seed;

    defaultSettings();
    moveToThread(this);
}

void Movement::run()
{
    qsrand(seed);
    startTimer(interval);
    exec();
}

void Movement::timerEvent(QTimerEvent *)
{
    int dx;
    int dy;
    int random;
    double keepWayProb = settings->value("KeepWayProb").toDouble();
    QPoint newPos;

    for (int i = iStart; i < iStop; i++) {
        do {
            newPos = ants->at(i)->getPosition();

            if ((double)qrand() / RAND_MAX < keepWayProb) {
                dx = ants->at(i)->old_dx;
                dy = ants->at(i)->old_dy;
            }
            else {
                random = (int)(((double)qrand() / RAND_MAX) * 8.0);

                switch (random) {
                case 0:
                    dx = -1;
                    dy = 0;
                    break;

                case 1:
                    dx = -1;
                    dy = -1;
                    break;

                case 2:
                    dx = 0;
                    dy = -1;
                    break;

                case 3:
                    dx = 1;
                    dy = -1;
                    break;

                case 4:
                    dx = 1;
                    dy = 0;
                    break;

                case 5:
                    dx = 1;
                    dy = 1;
                    break;

                case 6:
                    dx = 0;
                    dy = 1;
                    break;

                default:
                    dx = -1;
                    dy = 1;
                }
            }
            newPos.rx() += dx;
            newPos.ry() += dy;
        } while(!widget->insideMap(newPos));

        ants->at(i)->setPosition(newPos);
        widget->leavePheromone(newPos);
        ants->at(i)->old_dx = dx;
        ants->at(i)->old_dy = dy;
    }
}

void Movement::defaultSettings()
{
    settings->setValue("KeepWayProb", settings->value("KeepWayProb", 0.75));
}
