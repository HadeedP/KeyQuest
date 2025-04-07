/**
 * @file main.cpp
 * @brief Main entry point for the KeyQuest application
 * @author Alan Cruz
 * @details This file contains the main entry point for the KeyQuest application,
 *          initializing the Qt application framework and launching the main window.
 */

#include "mainwindow.h"

#include <QApplication>

/**
 * @brief Main entry point of the application
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return Exit code of the application
 * @details Initializes the Qt application with high DPI support,
 *          creates and displays the main window, and enters the event loop.
 */
int main(int argc, char *argv[])
{
    // Enable High DPI scaling
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
