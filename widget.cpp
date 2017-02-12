#include "widget.h"
#include <QtGui>

static const char settingsPath[] = "Settings.ini";

//--------------------- Constructor --------------------------------------------

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    unsigned int time = QTime::currentTime().msec();
    int minWidth;
    int minHeight;
    int nAnts;
    int nThreads;
    int start;
    int stop;
    int antsPerThread;
    int interval;

    image = NULL;
    settings = new QSettings(settingsPath, QSettings::IniFormat, this);
    defaultSettings();

    anthillPosition.setX(settings->value("Objects/AnthillX").toInt());
    anthillPosition.setY(settings->value("Objects/AnthillY").toInt());
    foodPosition.setX(settings->value("Objects/FoodX").toInt());
    foodPosition.setY(settings->value("Objects/FoodY").toInt());

    minWidth = settings->value("Map/MinWidth").toInt();
    minHeight = settings->value("Map/MinHeight").toInt();
    setMinimumWidth(minWidth);
    setMinimumHeight(minHeight);
    resize(minWidth, minHeight);

    setWindowTitle("Colonia de hormigas");
    setWindowIcon(QIcon(":/ant-icon.png"));

    nAnts = settings->value("Ants").toInt();

    for (int i = 0; i < nAnts; i++)
        ants += new Ant(anthillPosition);

    nThreads = settings->value("Threads").toInt();
    interval = settings->value("Timers/AntMovement").toInt();
    antsPerThread = nAnts / nThreads;
    start = 0;
    stop = antsPerThread;

    for (int i = 0; i < nThreads; i++) {
        threads += new Movement(this, ants, start, stop, interval, time + i);
        start = stop;
        stop += antsPerThread;
    }
}

Widget::~Widget()
{
    for (int i = threads.size() - 1; i >= 0; i--) {
        threads[i]->exit();
        threads[i]->wait();
    }

    delete image;
}

//--------------------- Public methods -----------------------------------------

bool Widget::insideMap(const QPoint &point)
{
    bool response = true;
    mutex.lock();

    if (point.x() < 0 || point.y() < 0)
        response = false;
    else if (point.x() >= width() || point.y() >= height())
        response = false;

    mutex.unlock();
    return response;
}

void Widget::leavePheromone(const QPoint &point)
{
    float quantity = settings->value("Map/AntPheromone").toFloat();
    float total;
    mutex.lock();

    total = pheromones[point.y() * width() + point.x()] + quantity;
    pheromones[point.y() * width() + point.x()] = total > 1.0 ? 1.0 : total;

    mutex.unlock();
}

void Widget::getPheromones(const QPoint &point, float data[])
{
    int x = point.x();
    int y = point.y();
    int offset;
    bool left;
    bool right;
    bool up;
    bool down;

    mutex.lock();

    offset = width();
    left = (x - 1) >= 0;
    right = (x + 1) < width();
    up = (y - 1) >= 0;
    down = (y + 1) < height();

    data[Center] = pheromones[y * offset + x];

    if (left)
        data[Left] = pheromones[y * offset + x - 1];
    else
        data[Left] = 0.0;

    if (left && up)
        data[LeftUp] = pheromones[(y - 1) * offset + x - 1];
    else
        data[LeftUp] = 0.0;

    if (up)
        data[Up] = pheromones[(y - 1) * offset + x];
    else
        data[Up] = 0.0;

    if (right && up)
        data[RightUp] = pheromones[(y - 1) * offset + x + 1];
    else
        data[RightUp] = 0.0;

    if (right)
        data[Right] = pheromones[y * offset + x + 1];
    else
        data[Right] = 0.0;

    if (right && down)
        data[RightDown] = pheromones[(y + 1) * offset + x + 1];
    else
        data[RightDown] = 0.0;

    if (down)
        data[Down] = pheromones[(y + 1) * offset + x];
    else
        data[Down] = 0.0;

    if (left && down)
        data[LeftDown] = pheromones[(y + 1) * offset + x - 1];
    else
        data[LeftDown] = 0.0;

    mutex.unlock();
}

//--------------------- Protected methods --------------------------------------

void Widget::resizeEvent(QResizeEvent *)
{
    mutex.lock();

    delete image;
    image = new QImage(size(), QImage::Format_RGB32);
    image->fill(Qt::white);
    pheromones.fill(0.0, width() * height());

    mutex.unlock();

    for (int i = ants.size() - 1; i >= 0; i--)
        ants[i]->setPosition(anthillPosition);
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    mutex.lock();
    painter.drawImage(0, 0, *image);
    mutex.unlock();

    painter.setPen(QPen(Qt::green, 10));
    painter.drawPoint(foodPosition);
    painter.setPen(QPen(Qt::blue, 5));
    painter.drawPoint(anthillPosition);
    painter.setPen(QPen(Qt::black, 2));

    for (int i = ants.size() - 1; i >= 0; i--)
        painter.drawPoint(ants[i]->getPosition());
}

void Widget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timerUpdating)
        update();
    else
        degradePheromone();
}

void Widget::showEvent(QShowEvent *)
{
    for (int i = threads.size() - 1; i >= 0; i--)
        threads[i]->start();

    timerUpdating = startTimer(settings->value("Timers/WidgetUpdating").toInt());
    timerDegrade = startTimer(settings->value("Timers/DegradePheromone").toInt());
}

//--------------------- Private methods ----------------------------------------

void Widget::defaultSettings()
{
    settings->setValue("Ants", settings->value("Ants", 1000));
    settings->setValue("Threads", settings->value("Threads", 2));

    settings->setValue("Map/MinWidth", settings->value("Map/MinWidth", 800));
    settings->setValue("Map/MinHeight", settings->value("Map/MinHeight", 600));
    settings->setValue("Map/AntPheromone", settings->value("Map/AntPheromone", 0.1));
    settings->setValue("Map/DegradeFactor", settings->value("Map/DegradeFactor", 1.01));

    settings->setValue("Objects/AnthillX", settings->value("Objects/AnthillX", 400));
    settings->setValue("Objects/AnthillY", settings->value("Objects/AnthillY", 400));
    settings->setValue("Objects/FoodX", settings->value("Objects/FoodX", 400));
    settings->setValue("Objects/FoodY", settings->value("Objects/FoodY", 100));

    settings->setValue("Timers/WidgetUpdating", settings->value("Timers/WidgetUpdating", 20));
    settings->setValue("Timers/AntMovement", settings->value("Timers/AntMovement", 10));
    settings->setValue("Timers/DegradePheromone", settings->value("Timers/DegradePheromone", 100));
}

void Widget::degradePheromone()
{
    float factor = settings->value("Map/DegradeFactor").toFloat();
    unsigned int tone;
    unsigned int color;

    mutex.lock();

    for (int i = 0; i < width(); i++) {
        for (int j = 0; j < height(); j++) {
            pheromones[j * width() + i] /= factor;
            tone = (unsigned int)((1.0 - pheromones[j * width() + i]) * 255.0);
            color = 0xFFFF0000 | (tone << 8) | tone;
            image->setPixel(i, j, color);
        }
    }

    mutex.unlock();
}
