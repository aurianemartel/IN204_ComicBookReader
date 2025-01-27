#include <QByteArray>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QDebug>

#include <unrar/rar.hpp>
#include <unrar/dll.hpp>

#include "comicCBR.hpp"
#include "gestionRAR.hpp"


// Méthodes MyComicCBR

MyComicCBR::MyComicCBR(QObject *parent, QString filePath,
                       void(*notifyPageLoaded)(QObject *parent, int pageNumber)) : MyComic(parent) {
    this->notifyPageLoaded = notifyPageLoaded;
    loadComic(filePath); 
}

// Load asynchrone : loadComic définit dans MyComic

void MyComicCBR::loadNames(QString filePath) {
    getSortedFileNamesRAR(filePath, names);
}   

// En cours !!!
void MyComicCBR::loadPagesAsync(QString filePath) {
    
    // Watcher ?

    QFuture<void> future = QtConcurrent::run([this, filePath]() {
        // Open rar
        HANDLE hRAR = openRAR(filePath);

        // Création espace de stockage des données et emplacement de chargement images
        QByteArray imageData;
        QPixmap pixmap;

        // Boucle d'ajout des pages à la liste
        for (int i=0; i<nbPages; ++i) {
            hRAR = getImageFromNameCBR(hRAR, filePath, names[i], imageData);
            if (hRAR  == NULL) {
                qWarning() << "Incorrect handle";
                return;
            } else {
                pixmap.loadFromData(imageData);
                pages[i] = pixmap;
            }

            // Trigger UI : QMetaObject
            (*notifyPageLoaded)(this->parent(), i);
        }
        RARCloseArchive(hRAR);
    });
    // Watcher ?
}


// Version 1 dans l'ordre, sans asynchronicité

void MyComicCBR::simpleLoadComic(QString filePath) {
    qDebug() << "Debug log: Entering simpleLoadComicCBR";
    // Clear previous data
    names.clear();
    pages.clear();

    // Get file names in names : première ouverture/fermeture de l'archive
    getSortedFileNamesRAR(filePath, names);
    nbPages = names.size(); // sinon pas de déplacement dans le fichier

    // Get files from file names
    HANDLE hRAR = openRAR(filePath);
    // Get : création QByteArray, création Pixmap, boucle sur names d'ajout à pages
    QByteArray imageData;
    QPixmap pixmap;

    for (int i=0; i<names.size(); ++i) {
        hRAR = getImageFromNameCBR(hRAR, filePath, names[i], imageData);
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