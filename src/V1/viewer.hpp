#ifndef VIEWER_HPP
#define VIEWER_HPP

#include <QWidget>
#include <QLabel>

#include "comic.hpp"


class MyViewer : public QWidget {
   Q_OBJECT

public :
    MyViewer(QWidget *parent = nullptr);
    int currentIndex = 0;
    void showCurrent();

protected :
    void keyPressEvent(QKeyEvent *event) override;

private slots :
    void browse();
    void next();
    void previous();
    void first();
    void last();

private :
    QLabel *imageBox;
    MyComic* comic = NULL;
    QString filePath = "";
    QPixmap placeholderPixmap;
};

#endif