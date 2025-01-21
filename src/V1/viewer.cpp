// #include <QApplication>
// #include <QCommandLineParser>
// #include <QCommandLineOption>
// #include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
// #include <QLineEdit>
#include <QPushButton>
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
    // Mise en place de l'organisation de la fenêtre
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    QVBoxLayout *menu = new QVBoxLayout();
    QVBoxLayout *left = new QVBoxLayout();
    QVBoxLayout *right = new QVBoxLayout();

    // Création des widgets
    imageBox = new QLabel("Image not loaded");
    imageBox->setAlignment(Qt::AlignCenter);

    QPushButton *loadButton = new QPushButton("Load");
    loadButton->setFixedSize(100, 25);
    QPushButton *previousButton = new QPushButton("<");
    previousButton->setFixedSize(50, 200);
    QPushButton *firstButton = new QPushButton("<<");
    firstButton->setFixedSize(50, 50);
    QPushButton *nextButton = new QPushButton(">");
    nextButton->setFixedSize(50, 200);
    QPushButton *lastButton = new QPushButton(">>");
    lastButton->setFixedSize(50, 50);

    // Montage des widgets dans la fenêtre
    menu->addWidget(loadButton, 0, Qt::AlignTop);
    mainLayout->addLayout(menu);

    left->addStretch(1);
    left->addWidget(previousButton, 0, Qt::AlignVCenter);
    left->addStretch(1);
    left->addWidget(firstButton, 0, Qt::AlignBottom);
    mainLayout->addLayout(left);

    mainLayout->addWidget(imageBox);

    right->addStretch(1);
    right->addWidget(nextButton, 0, Qt::AlignVCenter);
    right->addStretch(1);
    right->addWidget(lastButton, 0, Qt::AlignBottom);
    mainLayout->addLayout(right);
}

void MyViewer::browse() {}

void MyViewer::showCurrent() {}

void MyViewer::next() {}

void MyViewer::previous() {}

void MyViewer::first() {}

void MyViewer::last() {}