#include <QByteArray>
#include <QFile>
#include <QTemporaryFile>
#include <QDebug>

#include <unrar/rar.hpp>
#include <unrar/dll.hpp>

#include "gestionRAR.hpp"


// Fonctions privées : gestion du RAR

//Ouverture de l'archive rar
HANDLE openRAR(QString filePath) {
    qDebug() << "Debug log: Entering openRAR" << filePath;
    RAROpenArchiveDataEx archiveData = {};
    archiveData.ArcName = (char *) filePath.toUtf8().constData();
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
void getSortedFileNamesRAR(QString filePath, QStringList& names) {
    qDebug() << "Debug log: Entering getSortedFileNamesRAR";

    names.clear();

    // Ouverture du rar
    HANDLE hRAR = openRAR(filePath);

    // Boucle de lecture fichiers + ajout
    struct RARHeaderDataEx headerData = {};
    while (RARReadHeaderEx(hRAR, &headerData) == 0) {
        QString fileName = QString::fromUtf8(headerData.FileName);
        // Vérifier que c'est bien une image d'abord
        if (fileName.endsWith(".jpg", Qt::CaseInsensitive) || 
            fileName.endsWith(".jpeg", Qt::CaseInsensitive) || 
            fileName.endsWith(".bmp", Qt::CaseInsensitive) || 
            fileName.endsWith(".png", Qt::CaseInsensitive)) {
            names.append(fileName);
        } else {
            qWarning() << "Ignoring <<" << fileName <<">>";
        } 
        RARProcessFile(hRAR, RAR_SKIP, NULL, NULL);
    }

    // Fermeture rar
    RARCloseArchive(hRAR);

    // Tri
    names.sort();
}


//Récupération du contenu d'un fichier quand on est au bon header
bool extractOneFile(HANDLE hRAR, QByteArray& fileData) {
    // Créer un fichier temporaire
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
HANDLE getImageFromNameCBR(HANDLE hRAR, QString filePath, QString fileName, QByteArray& fileData) {
    //qDebug() << QString("Debug log: Entering getImageFromNameCBR, fileName : %1").arg(fileName);
    fileData.clear();
    bool reloop = false;
    struct RARHeaderDataEx headerData = {};
    HANDLE locHRAR = hRAR;

    while(true) {
        //if fin archive : fermer puis ouvrir
        if (RARReadHeaderEx(locHRAR, &headerData) != 0) {
            if (reloop) {
                // Cas de boucle infinie
                qWarning() << "File not found " << fileName;
                return NULL;
                }
            // fermer puis ouvrir
            RARCloseArchive(locHRAR);
            qDebug() << "Reopening here : " << fileName;
            locHRAR = openRAR(filePath);
            if (locHRAR == NULL) qDebug() << "Reopened and got NULL!";
            reloop = true;
        } else {
            //if fini (j'ai mon fichier) : remplir fileData et return handle actuel
            if (strcmp(fileName.toUtf8().constData(),
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