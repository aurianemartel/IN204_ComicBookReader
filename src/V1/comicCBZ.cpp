#include <QByteArray>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QDebug>

#include <quazip5/quazip.h>
#include <quazip5/quazipfile.h>

// Pour tester asynchronicité :
//#include <thread>
//#include <chrono>

#include "comicCBZ.hpp"

// Méthodes MyComicCBZ

MyComicCBZ::MyComicCBZ(QObject *parent, QString filePath) : MyComic(parent) {
    loadComic(filePath); 
}

// Load asynchrone : loadComic définit dans MyComic

void MyComicCBZ::loadNames(QString filePath) {
    QuaZip zip(filePath);
    if (!zip.open(QuaZip::mdUnzip)) {
        qWarning() << "Failed to open with filePath : " << filePath;
        return;
    }

    // Load names
    bool nextFile = zip.goToFirstFile();
    while(nextFile) {
        QuaZipFile file(&zip);
        file.open(QIODevice::ReadOnly);

        QString fileName = file.getActualFileName();
        if (fileName.endsWith(".jpg", Qt::CaseInsensitive) || 
            fileName.endsWith(".jpeg", Qt::CaseInsensitive) || 
            fileName.endsWith(".bmp", Qt::CaseInsensitive) || 
            fileName.endsWith(".png", Qt::CaseInsensitive)) {
            names.append(fileName);
        }
        file.close();
        nextFile = zip.goToNextFile();
    }
    zip.close();
}

void MyComicCBZ::loadPagesAsync(QString filePath) {
    QFuture<void> future = QtConcurrent::run([this, filePath]() {
        QuaZip zip(filePath);
        zip.open(QuaZip::mdUnzip);

        // Création espace de stockage des données et emplacement de chargement images
        QByteArray imageData;
        QPixmap pixmap;

        // Boucle d'ajout des pages à la liste
        for (int i = 0; i < nbPages; ++i) {
            zip.setCurrentFile(names[i]);
            QuaZipFile file(&zip);
            file.open(QIODevice::ReadOnly);

            imageData = file.readAll();
            pixmap.loadFromData(imageData);
            pages[i] = pixmap;
            
            file.close();

            // Notify viewer
            useNotifyLoading(i);
            //std::this_thread::sleep_for(std::chrono::seconds(5));
        }
        zip.close();
    });
}


// Version 1 simple, sans asynchronicité

void MyComicCBZ::simpleLoadComic(QString filePath) {
    QuaZip zip(filePath);
    zip.open(QuaZip::mdUnzip);
    
    names.clear();
    pages.clear();

    bool more = zip.goToFirstFile();
    while(more) {
        QuaZipFile file(&zip);
        file.open(QIODevice::ReadOnly);

        QString fileName = file.getActualFileName();
        if (fileName.endsWith(".jpg", Qt::CaseInsensitive) || 
            fileName.endsWith(".jpeg", Qt::CaseInsensitive) || 
            fileName.endsWith(".bmp", Qt::CaseInsensitive) || 
            fileName.endsWith(".png", Qt::CaseInsensitive)) {
            
            names.append(fileName);

            QByteArray imageData = file.readAll();
            QPixmap pixmap;
            pixmap.loadFromData(imageData);

            pages.append(pixmap);
        }
        file.close();
        ++nbPages;
        more = zip.goToNextFile();
    }
    zip.close();
}
