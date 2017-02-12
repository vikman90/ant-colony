#include "mainwindow.h"
#include "global.h"
#include <QtGui>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.btSimulate, SIGNAL(clicked()), SLOT(simulate()));
}

void MainWindow::timerEvent(QTimerEvent *)
{
    update();
}

void MainWindow::simulate()
{
    int nAnts = ui.ants->value();
    int width = ui.mapWidth->value();
    int height = ui.mapHeight->value();
    QPoint foodPosition(ui.foodX->value(), ui.foodY->value());
    QPoint anthillPosition(ui.anthillX->value(), ui.anthillY->value());

    QVBoxLayout *layout = new QVBoxLayout(ui.frMap);
    map = new Map(nAnts, width, height, foodPosition, anthillPosition);
    layout->addWidget(map);

    ui.frControl->setEnabled(false);
    startTimer(WINDOW_INTERVAL);
    map->runSimulation();
}
