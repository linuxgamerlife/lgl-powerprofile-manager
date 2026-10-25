#include <QApplication>
#include <QIcon>
#include <QLockFile>
#include <QStandardPaths>
#include "mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("LGL Power Profile Manager");
    app.setApplicationVersion("1.1.2");
    app.setOrganizationName("LinuxGamerLife");

    QLockFile lockFile(QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                       + "/lgl-powerprofile-manager.lock");
    if (!lockFile.tryLock(100))
        return 0;

    app.setWindowIcon(QIcon(":/lgl-powerprofile-manager.png"));

    MainWindow w;
    w.show();
    return app.exec();
}
