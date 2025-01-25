#include <QByteArray>
#include <QDir>
#include <QStringList>
#include <QFile>
#include <QDebug>
#include <QTemporaryFile>

#include <quazip5/quazip.h>
#include <quazip5/quazipfile.h>
#include <archive.h>
#include <archive_entry.h>
#include <unrar/rar.hpp>
#include <unrar/dll.hpp>

#include "comic.hpp"


// Fonctions privées : gestion du RAR
//Ouverture de l'archive rar
HANDLE openRAR(QString filePath, RAROpenArchiveDataEx& archiveData) {
    qDebug() << "Debug log: Entering openRAR" << filePath;
    const char* archiveName = filePath.toUtf8().constData();
    //RAROpenArchiveDataEx archiveData = {};
    archiveData.ArcName = (char *) archiveName;
    archiveData.OpenMode = RAR_OM_EXTRACT;
    
    HANDLE hArc = RAROpenArchiveEx(&archiveData);
    if(hArc == NULL || archiveData.OpenResult !=0) {
        qWarning("Erreur d'ouverture de l'archive");
        return NULL;
    }
    // qDebug() << "Debug log: Opened " << hArc;
    return hArc;
}

//Récupération dans une QStringList des noms de fichiers triés du RAR
void getSortedFileNamesRAR(QString filePath, QStringList& zipFiles) {
    qDebug() << "Debug log: Entering getSortedFileNamesRAR";

    zipFiles.clear();

    // Ouverture du rar
    RAROpenArchiveDataEx archiveData = {};
    HANDLE hRAR = openRAR(filePath, archiveData);

    // Boucle de lecture fichiers + ajout
    struct RARHeaderDataEx headerData = {};
    while (RARReadHeaderEx(hRAR, &headerData) == 0) {
        QString fileName = QString::fromUtf8(headerData.FileName);
        // Vérifier que c'est bien une image d'abord
        if (fileName.endsWith(".jpg", Qt::CaseInsensitive) || 
            fileName.endsWith(".jpeg", Qt::CaseInsensitive) || 
            fileName.endsWith(".bmp", Qt::CaseInsensitive) || 
            fileName.endsWith(".png", Qt::CaseInsensitive)) {
            zipFiles.append(fileName);
        } else {
            qWarning() << "Ignoring <<" << fileName <<">>";
        } 
        RARProcessFile(hRAR, RAR_SKIP, NULL, NULL);
    }

    // Fermeture rar
    RARCloseArchive(hRAR);

    // Tri
    zipFiles.sort();
}


//Récupération du contenu d'un fichier quand on est au bon header
bool extractOneFile(HANDLE hRAR, QByteArray& fileData) {
    // Créer un fichier temporaireqDebug() << std::format("Debug log: Entering getImageFromNameCBR, fileName : {}", fileName);
    QTemporaryFile tempFile;
    if (!tempFile.open()) {
        qWarning("Impossible de créer un fichier temporaire.");
        return false;
    }

    QString tempFilePath = tempFile.fileName();
    tempFile.close(); // Nous fermons car RARProcessFile écrira directement sur ce chemin.

    // Extraire le fichier dans le fichier temporaire
    int result = RARProcessFile(hRAR, RAR_EXTRACT, nullptr, tempFilePath.toLocal8Bit().data());
    if (result != 0) {
        qWarning("Erreur lors de l'extraction du fichier : %d", result);
        return false;
    }

    // Ouvrir le fichier temporaire pour lire son contenu
    QFile extractedFile(tempFilePath);
    if (!extractedFile.open(QIODevice::ReadOnly)) {
        qWarning("Impossible d'ouvrir le fichier temporaire extrait : %s", qPrintable(tempFilePath));
        return false;
    }

    // Lire le contenu du fichier dans le QByteArray
    fileData = extractedFile.readAll();
    extractedFile.close();

    // Supprimer le fichier temporaire (QTemporaryFile s'en occupe déjà normalement)
    if (!QFile::remove(tempFilePath)) {
        qWarning("Impossible de supprimer le fichier temporaire : %s", qPrintable(tempFilePath));
    }

    return true;
}

//Récupérer une page dans un CBR à partir de son nom
HANDLE getImageFromNameCBR(HANDLE hRAR, 
                           QString filePath, QString fileName, QByteArray& fileData,
                           RAROpenArchiveDataEx& archiveData) {
    qDebug() << QString("Debug log: Entering getImageFromNameCBR, fileName : %1").arg(fileName);
    const char* targetFileName = fileName.toUtf8().constData();
    fileData.clear();
    bool reloop = false;
    struct RARHeaderDataEx headerData = {};
    HANDLE locHRAR = hRAR;


    while(true) {
        //if fin archive : fermer puis ouvrir
        if (RARReadHeaderEx(locHRAR, &headerData) != 0) {
            if (reloop) {
                // Cas de boucle infinie
                qWarning() << "File not found " << targetFileName;
                return NULL;
                }
            // fermer puis ouvrir
            RARCloseArchive(locHRAR);
            qDebug() << "Traget 1 : " << targetFileName << " End target 1";
            locHRAR = openRAR(filePath, archiveData);
            if (locHRAR == NULL) qDebug() << "Reopened and got NULL!";
            reloop = true;
            qDebug() << "Traget 2 : " << targetFileName << " End target 2";
        } else {
            //qDebug() << "-- " << headerData.FileName;
            //if fini (j'ai mon fichier) : remplir fileData et return handle actuel
            if (strcmp(fileName.toUtf8().constData(),
                        // targetFileName, 
                        headerData.FileName) == 0) {
                // Read the file data
                if(!extractOneFile(locHRAR,fileData)) {
                    qWarning("Could not extract file");
                    return NULL;
                };
                return locHRAR;
            }
            // Passer au fichier suivant
            RARProcessFile(locHRAR, RAR_SKIP, NULL, NULL);
        }
    }
}


// Méthodes MyComic

MyComic::MyComic(QObject *parent) : QObject(parent) {}

QPixmap* MyComic::getPage(int whichPage) {
    return &pages[whichPage];
}

int MyComic::getNbPages() {
    return nbPages;
}


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


// Méthodes MyComicCBR

MyComicCBR::MyComicCBR(QObject *parent, QString filePath) : MyComic(parent) {
    loadComic(filePath); 
}


/* load en libarchive, fonctionne mais pas dans l'ordre
void MyComicCBR::loadComic(QString filePath) {
    // Clear previous data
    zipFiles.clear();
    pages.clear();

    // Open the RAR archive
    struct archive *a = archive_read_new();
    archive_read_support_format_rar(a);

    if (archive_read_open_filename(a, filePath.toStdString().c_str(), 10240) != ARCHIVE_OK) {
        //qWarning() << "Failed to open RAR file:" << filePath;
        archive_read_free(a);
        return;
    }

    struct archive_entry *entry;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        QString fileName = QString::fromUtf8(archive_entry_pathname(entry));
        QString lowerFileName = fileName.toLower();

        // Check if the file is an image
        if (lowerFileName.endsWith(".jpg") || lowerFileName.endsWith(".jpeg") ||
            lowerFileName.endsWith(".png") || lowerFileName.endsWith(".bmp")) {
            
            // Read the file data
            QByteArray fileData;
            const void *buffer;
            size_t size;
            la_int64_t offset;

            while (archive_read_data_block(a, &buffer, &size, &offset) == ARCHIVE_OK) {
                fileData.append(static_cast<const char *>(buffer), size);
            }

            // Load the image data into a QPixmap
            QPixmap pixmap;
            if (pixmap.loadFromData(fileData)) {
                pages.append(pixmap);
                zipFiles.append(fileName);
            } else {
                //qWarning() << "Failed to load image from file:" << fileName;
            }
        }
    }

    // Cleanup
    archive_read_free(a);

    // Update the page count
    nbPages = pages.size();
}
*/

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
    RAROpenArchiveDataEx archiveData = {};
    HANDLE hRAR = openRAR(filePath, archiveData);
    // Get : création QByteArray, création Pixmap, boucle sur zipFiles d'ajout à pages
    QByteArray imageData;
    QPixmap pixmap;

    for (int i=0; i<zipFiles.size(); ++i) {
        hRAR = getImageFromNameCBR(hRAR, filePath, zipFiles[i], imageData, archiveData);
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