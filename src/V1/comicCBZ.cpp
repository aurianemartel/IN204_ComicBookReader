#include <QByteArray>
#include <QDebug>

#include <quazip5/quazip.h>
#include <quazip5/quazipfile.h>

#include "comicCBZ.hpp"

// Méthodes MyComicCBZ

MyComicCBZ::MyComicCBZ(QObject *parent, QString filePath) : MyComic(parent) {
    loadComic(filePath); 
}

void MyComicCBZ::loadComic(QString filePath) {
    QuaZip zip(filePath);
    zip.open(QuaZip::mdUnzip);
    
    zipFiles.clear();
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
            
            zipFiles.append(fileName);

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
