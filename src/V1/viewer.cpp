// #include <QApplication>
// #include <QCommandLineParser>
// #include <QCommandLineOption>
// #include <QWidget>
#include <QVBoxLayout>
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


MyViewer::MyViewer(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    imageLabel = new QLabel("Image not loaded");
    imageLabel->setAlignment(Qt::AlignCenter);

    mainLayout->addWidget(imageLabel);
}

void MyViewer::browse() {}

void MyViewer::showCurrent() {}

void MyViewer::next() {}

void MyViewer::previous() {}

void MyViewer::first() {}

void MyViewer::last() {}