#include "mainwindow.h"
#include "log_manager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Initialize Logger
    LogManager::instance();

    MainWindow w;
    w.show();
    return a.exec();
}

// 
