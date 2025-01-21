#include <QApplication>
// #include <QCommandLineParser>
// #include <QCommandLineOption>
// #include <QWidget>
// #include <QVBoxLayout>
// #include <QHBoxLayout>
// #include <QLabel>
// #include <QLineEdit>
// #include <QPushButton>
// #include <QPixmap>
// #include <QMessageBox>
// #include <QFileDialog>
// #include <quazip5/quazip.h>
// #include <quazip5/quazipfile.h>
// #include <QtConcurrent/QtConcurrent>
// #include <QFutureWatcher>
// #include <QPainter>

// #include <thread>
// #include <chrono>

#include "viewer.hpp"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MyViewer viewer;

    viewer.setWindowTitle("Image Viewer");
    viewer.resize(800, 600);
    viewer.show();
    return app.exec();
}

