#include <QByteArray>
#include <QDir>
#include <QStringList>
#include <QFile>

#include <quazip5/quazip.h>
#include <quazip5/quazipfile.h>
#include <archive.h>
#include <archive_entry.h>

#include "comic.hpp"


MyComic::MyComic(QObject *parent) : QObject(parent) {}

QPixmap* MyComic::getPage(int whichPage) {
    return &pages[whichPage];
}

int MyComic::getNbPages() {
    return nbPages;
}

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

MyComicCBR::MyComicCBR(QObject *parent, QString filePath) : MyComic(parent) {
    loadComic(filePath); 
}

//void MyComicCBR::loadComic(QString filePath) {}





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
