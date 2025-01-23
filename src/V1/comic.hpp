#ifndef COMIC_HPP
#define COMIC_HPP

#include <QWidget>
#include <QPixmap>

class MyComic : public QObject {
   Q_OBJECT

public :
    MyComic(QObject *parent = nullptr);

    QPixmap* getPage(int whichPage);
    int getNbPages();
protected :
    int nbPages = 0;
    QList<QPixmap> pages;
    QStringList zipFiles;

    virtual void loadComic(QString filePath) = 0;
};

class MyComicCBZ : public MyComic {
    Q_OBJECT

public :
    MyComicCBZ(QObject *parent = nullptr, QString filePath = "");

private :
    void loadComic(QString filePath);
};

class MyComicCBR : public MyComic {
    Q_OBJECT

public :
    MyComicCBR(QObject *parent = nullptr, QString filePath = "");

private :
    void loadComic(QString filePath);
};

#endif




