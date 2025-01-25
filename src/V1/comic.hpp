#ifndef COMIC_HPP
#define COMIC_HPP

#include <QWidget>
#include <QPixmap>
#include <QStringList>

class MyComic : public QObject {
   Q_OBJECT

public :
    MyComic(QObject *parent = nullptr);
    
    static MyComic* createComic(QObject *parent = nullptr, QString filePath = "");

    QPixmap* getPage(int whichPage);
    int getNbPages();
protected :
    int nbPages = 0;
    QList<QPixmap> pages;
    QStringList zipFiles;

    virtual void loadComic(QString filePath) = 0;
};

#endif




