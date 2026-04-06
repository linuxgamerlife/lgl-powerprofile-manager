#include <QApplication>
#include <QIcon>
#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("LGL Power Profile Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("LinuxGamerLife");

    app.setWindowIcon(QIcon(":/lgl-powerprofile-manager.png"));

    MainWindow w;
    w.show();
    return app.exec();
}
