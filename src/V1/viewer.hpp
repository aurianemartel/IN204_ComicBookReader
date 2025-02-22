#ifndef VIEWER_HPP
#define VIEWER_HPP

#include <QWidget>
#include <QLabel>
#include <QResizeEvent>

#include "comic.hpp"

class MyViewer : public QWidget {
   Q_OBJECT

public :
    MyViewer(QWidget *parent = nullptr);
    
    void notifyLoading(int pageNumber);

protected :
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

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
    QPixmap placeholderPixmap;
};

#endif