#include <QByteArray>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QDebug>

#include <quazip5/quazip.h>
#include <quazip5/quazipfile.h>

#include "comicCBZ.hpp"

// Méthodes MyComicCBZ

MyComicCBZ::MyComicCBZ(QObject *parent, QString filePath,
                       void(*notifyPageLoaded)(QObject* parent, int pageNumber)) : MyComic(parent) {
    this->notifyPageLoaded = notifyPageLoaded; 
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

// À faire
void MyComicCBZ::loadPagesAsync(QString filePath) {

}


// Load simple, sans asynchronicité

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
