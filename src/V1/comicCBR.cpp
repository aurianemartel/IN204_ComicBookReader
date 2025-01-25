#include <QByteArray>
#include <QDebug>

#include <unrar/rar.hpp>
#include <unrar/dll.hpp>

#include "comicCBR.hpp"
#include "gestionRAR.hpp"


// Méthodes MyComicCBR

MyComicCBR::MyComicCBR(QObject *parent, QString filePath) : MyComic(parent) {
    loadComic(filePath); 
}

// Load en unrar dans l'ordre

void MyComicCBR::loadComic(QString filePath) {
    qDebug() << "Debug log: Entering loadComicCBR";
    // Clear previous data
    zipFiles.clear();
    pages.clear();

    // Get file names in zipFiles : première ouverture/fermeture de l'archive
    getSortedFileNamesRAR(filePath, zipFiles);
    nbPages = zipFiles.size(); // sinon pas de déplacement dans le fichier

    // Get files from file names
    HANDLE hRAR = openRAR(filePath);
    // Get : création QByteArray, création Pixmap, boucle sur zipFiles d'ajout à pages
    QByteArray imageData;
    QPixmap pixmap;

    for (int i=0; i<zipFiles.size(); ++i) {
        hRAR = getImageFromNameCBR(hRAR, filePath, zipFiles[i], imageData);
        if (hRAR  == NULL) {
            qWarning() << "Incorrect handle";
            return;
        } else {
            qWarning() << "Got some bytes: " << imageData.size();
            pixmap.loadFromData(imageData);
            pages.append(pixmap);
            qWarning() << "Got pixmap: " << pages[i].width() << " x " << pages[i].height();
        }
    }

    qWarning() << "Got some pictures: " << pages.size();

    // Close
    RARCloseArchive(hRAR);
}