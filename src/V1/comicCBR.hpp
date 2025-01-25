#ifndef COMICCBR_HPP
#define COMICCBR_HPP

#include "comic.hpp"

class MyComicCBR : public MyComic {
    Q_OBJECT

public :
    MyComicCBR(QObject *parent = nullptr, QString filePath = "");

private :
    void loadComic(QString filePath);
};

#endif