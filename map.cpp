#include "map.h"
#include "global.h"
#include <QtGui>

//--------------------- Constructor --------------------------------------------

Map::Map(int nAnts, int width, int height, const QPoint &foodPos, const QPoint &anthillPos, QWidget *parent) : QWidget(parent)
{
    int time = QTime::currentTime().msec();

    state = Paused;
    map_width = width;
    map_height = height;
    foodPosition = foodPos;
    anthillPosition = anthillPos;

    image = new QImage(width, height, QImage::Format_RGB32);
    pheromones.fill(0.0, width * height);

    for (int i = 0; i < nAnts; i++)
        ants.append(new Ant(this, time + i, anthillPosition));
}

Map::~Map()
{
    delete image;

    for (int i = ants.size() - 1; i >= 0; i--) {
        ants[i]->quit();
        ants[i]->wait();
    }

}

//--------------------- Public methods -----------------------------------------

bool Map::insideMap(const QPoint &point)
{
    bool response = true;

    if (point.x() < 0 || point.y() < 0)
        response = false;
    else if (point.x() >= map_width || point.y() >= map_height)
        response = false;

    return response;
}

void Map::leavePheromone(const QPoint &point, float quantity)
{
    float total;
    mutex.lock();

    total = pheromones[point.y() * map_width + point.x()] + quantity;
    pheromones[point.y() * map_width + point.x()] = total > 1.0 ? 1.0 : total;

    mutex.unlock();
}

void Map::getPheromones(const QPoint &point, float data[])
{
    int x = point.x();
    int y = point.y();
    int offset;
    bool left;
    bool right;
    bool up;
    bool down;

    mutex.lock();

    offset = map_width;
    left = (x - 1) >= 0;
    right = (x + 1) < map_width;
    up = (y - 1) >= 0;
    down = (y + 1) < map_height;

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

void Map::updatePheromone(float factor)
{
    QColor phColor(255, 0, 0);
    unsigned int tone;
    unsigned int color;

    mutex.lock();

    for (int i = 0; i < map_width; i++) {
        for (int j = 0; j < map_height; j++) {
            pheromones[j * map_width + i] /= factor;
            tone = (unsigned int)((1.0 - pheromones[j * map_width + i]) * 255.0);
            color = 0xFFFF0000 | (tone << 8) | tone;
            image->setPixel(i, j, color);
        }
    }

    mutex.unlock();
}

//--------------------- Public slots -------------------------------------------

void Map::runSimulation()
{
    state = Running;

    for (int i = ants.size() - 1; i >= 0; i--)
        ants[i]->start();

    startTimer(MAP_DEGRADEPHER_INTERVAL);
}

void Map::pauseSimulation()
{

}

void Map::waitForFood()
{

}

void Map::waitForAnthill()
{

}

//--------------------- Protected methods --------------------------------------

void Map::resizeEvent(QResizeEvent *event)
{

}

void Map::paintEvent(QPaintEvent *)
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

void Map::timerEvent(QTimerEvent *)
{
    updatePheromone(MAP_DEGRADEPHER_FACTOR);
}
