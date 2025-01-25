#include <QDebug>

#include "comic.hpp"
#include "comicCBR.hpp"
#include "comicCBZ.hpp"


// Méthodes MyComic

MyComic::MyComic(QObject *parent) : QObject(parent) {}

MyComic* MyComic::createComic(QObject *parent, QString filePath) {
    if (filePath.endsWith(".cbz", Qt::CaseInsensitive) ||
        filePath.endsWith(".zip", Qt::CaseInsensitive)) {
        return new MyComicCBZ(parent, filePath);
    } else if (filePath.endsWith(".cbr", Qt::CaseInsensitive) ||
                filePath.endsWith(".rar", Qt::CaseInsensitive)) {
        return new MyComicCBR(parent, filePath);
    } else {
        qWarning() << "Unrecognized file type for file : " << filePath;
        return NULL;
    }
}

QPixmap* MyComic::getPage(int whichPage) {
    return &pages[whichPage];
}

int MyComic::getNbPages() {
    return nbPages;
}