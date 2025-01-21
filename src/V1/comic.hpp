#ifndef COMIC_HPP
#define COMIC_HPP

#include <QWidget>
#include <QPixmap>

class MyComic : public QWidget {
   Q_OBJECT

public :
    MyComic(QWidget *parent = nullptr);


private :
    int nbPages;
    QList<QPixmap> pages;
};

#endif




