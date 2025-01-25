#ifndef VIEWER_HPP
#define VIEWER_HPP

#include <QWidget>
#include <QLabel>

#include "comic.hpp"


class MyViewer : public QWidget {
   Q_OBJECT

public :
    MyViewer(QWidget *parent = nullptr);

protected :
    void keyPressEvent(QKeyEvent *event) override;

private slots :
    void browse();
    void showCurrent();
    void next();
    void previous();
    void first();
    void last();

private :
    int currentIndex = 0;
    QLabel *imageBox;
    MyComic* comic = NULL;
    QString filePath = "";
};

#endif