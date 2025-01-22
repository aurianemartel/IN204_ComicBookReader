#ifndef COMIC_HPP
#define COMIC_HPP

#include <QWidget>
#include <QPixmap>

class MyComic : public QWidget {
   Q_OBJECT

public :
    MyComic(QWidget *parent = nullptr);
    MyComic(QWidget *parent = nullptr, QString filePath = "");

    QPixmap* getPage(int whichPage);
    int getNbPages();
private :
    int nbPages = 0;
    QList<QPixmap> pages;
    QStringList zipFiles;

    void loadComic(QString filePath);
};

#endif




