#include <QtGui/QApplication>
#include <QSettings>
#include <QCoreApplication>
#include <QSize>
#include <QDebug>
#include <QStringList>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("QCoverage");
    QCoreApplication::setOrganizationName("Filip Sobalski");
    QCoreApplication::setApplicationVersion("v1.0 alpha");

    QSettings settings;

    settings.setValue("dropShadowColor", "#000000");
    settings.setValue("normalNodeColor", "#eeeeee");
    settings.setValue("startNodeColor", "#386fd6");
    settings.setValue("endNodeColor", "#ec7575");
    settings.setValue("startEndNodeColor", "#aa00aa");
    settings.setValue("selectedColor", "#39b51c");
    settings.setValue("insertingLineColor", "#fdd000");
    settings.setValue("gridColor", "#eeeeff");
    settings.setValue("highlightColor", "#fdd000");
    settings.setValue("viewModeBcgColor", "#ffeeee");
    settings.setValue("defaultSceneSize", QSize(1500, 1500));

    MainWindow w;
    w.show();

    QStringList args = qApp->arguments();

    if(args.size() > 1)
	    w.loadFromFile(args.last());

    return a.exec();
}
