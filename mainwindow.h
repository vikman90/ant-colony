#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "map.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

protected:
    void timerEvent(QTimerEvent *);

private slots:
    void simulate();

private:
    Ui::MainWindow ui;
    Map *map;
};

#endif // MAINWINDOW_H
