#include <QDebug>

#include "comic.hpp"
#include "comicCBR.hpp"
#include "comicCBZ.hpp"


// Méthodes MyComic

MyComic::MyComic(QObject *parent) : QObject(parent) {}

MyComic* MyComic::createComic(QObject *parent, QString filePath,
                              void (*notifyPageLoaded)(QObject *parent, int pageNumber)) {
    if (filePath.endsWith(".cbz", Qt::CaseInsensitive) ||
        filePath.endsWith(".zip", Qt::CaseInsensitive)) {
        return new MyComicCBZ(parent, filePath, notifyPageLoaded);
    } else if (filePath.endsWith(".cbr", Qt::CaseInsensitive) ||
                filePath.endsWith(".rar", Qt::CaseInsensitive)) {
        return new MyComicCBR(parent, filePath, notifyPageLoaded);
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

// Load asynchrone

void MyComic::loadComic(QString filePath) {
    if (filePath.isEmpty()) {
        qWarning() << "Empty filePath";
        return;
    }

    // Clear previous data
    names.clear();
    pages.clear();

    // Load names : première boucle, première ouverture/fermeture de l'archive
    loadNames(filePath);

    if (names.isEmpty()) {
        qWarning() << "Empty file : "  << filePath;
        return;
    }

    nbPages = names.size();

    // Prepare pixmap space for the pages
    for (int i=0; i<nbPages; ++i) {
        pages.append(QPixmap());
    }

    // Load the pages
    loadPagesAsync(filePath);
}
