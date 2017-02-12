#include <QApplication>
#include <QSplashScreen>
#include <QTimer>
#include "widget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QSplashScreen splash(QPixmap(":/ant.gif"), Qt::WindowStaysOnTopHint);
    QTimer::singleShot(1000, &splash, SLOT(close()));
    splash.show();
    app.processEvents();

    Widget widget;
    widget.show();
    return app.exec();
}
