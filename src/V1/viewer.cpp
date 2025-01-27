// #include <QApplication>
// #include <QCommandLineParser>
// #include <QCommandLineOption>
// #include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
// #include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QPainter>
#include <QKeyEvent>
#include <QDebug>

// #include <thread>
// #include <chrono>

#include "viewer.hpp"


MyViewer::MyViewer(QWidget *parent) : QWidget(parent) {
    qDebug() << "Opening viewer";

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

    // Connection des boutons avec fonctions "slots"
    connect(loadButton, &QPushButton::clicked, this, &MyViewer::browse);
    connect(nextButton, &QPushButton::clicked, this, &MyViewer::next);
    connect(previousButton, &QPushButton::clicked, this, &MyViewer::previous);
    connect(firstButton, &QPushButton::clicked, this, &MyViewer::first);
    connect(lastButton, &QPushButton::clicked, this, &MyViewer::last);

    // Accepter événements clavier
    setFocusPolicy(Qt::StrongFocus);

    // Initialiser placeholder
    placeholderPixmap = QPixmap(800, 600);
    placeholderPixmap.fill(Qt::gray);
    QPainter painter(&placeholderPixmap);
    painter.drawText(placeholderPixmap.rect(), Qt::AlignCenter, "Please wait while loading...");

}

void MyViewer::keyPressEvent(QKeyEvent *event) {
    QMessageBox::StandardButton reply;
    switch (event->key()) {
        case Qt::Key_Right : 
        case Qt::Key_Space :
            qDebug() << "Right arrow or space bar pressed";
            next();
            break;
        case Qt::Key_Left :
            qDebug() << "Left arrow pressed";
            previous();
            break;
        case Qt::Key_PageUp :
            qDebug() << "PageUp arrow pressed";
            first();
            break;
        case Qt::Key_PageDown :
            qDebug() << "PageDown arrow pressed";
            last();
            break;
        case Qt::Key_Return :
            qDebug() << "Enter key pressed";
            browse();
            break;
        case Qt::Key_Escape :
            qDebug() << "Escape key pressed";
            reply = QMessageBox::question(this, "Quitter", 
                "Voulez-vous vraiment quitter ?", 
                QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                close();
            }
            break;
        default :
            QWidget::keyPressEvent(event);
    }
}

void MyViewer::notifyLoading(int pageNumber) {
    if (currentIndex == pageNumber) {
        showCurrent();
    }
}

void MyViewer::browse() {
    qDebug() << "Debug log: Entering browse";
    MyComic* newComic;

    filePath = QFileDialog::getOpenFileName(this, "Open Archive File", "", "Archives (*.zip);;Comics in CBZ format (*.cbz);;Comics in CBR format (*.cbr)");
    if (!filePath.isEmpty()) {
        newComic = MyComic::createComic(this, filePath);
        if (newComic == NULL) {
            QMessageBox::information(nullptr, "Warning", "Cannot open this file");
        } else {
            delete comic;
            comic = newComic;
            currentIndex = 0;
            showCurrent();
        }
    }
}

void MyViewer::showCurrent() {
    if (comic == NULL) return;
    QPixmap* pixmap = comic->getPage(currentIndex);
    if (pixmap->isNull()) {
        imageBox->setPixmap(placeholderPixmap.scaled(imageBox->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        imageBox->setPixmap(pixmap->scaled(imageBox->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void MyViewer::next() {
    if (comic != NULL && currentIndex < comic->getNbPages()-1) {
        ++currentIndex;
        showCurrent();
    }
}

void MyViewer::previous() {
    if (currentIndex > 0) {
        --currentIndex;
        showCurrent();
    }
}

void MyViewer::first() {
    currentIndex = 0;
    showCurrent();
}

void MyViewer::last() {
    if (comic == NULL) return;
    currentIndex = comic->getNbPages()-1;
    showCurrent();
}