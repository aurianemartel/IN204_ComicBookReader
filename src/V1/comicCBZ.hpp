#ifndef COMICCBZ_HPP
#define COMICCBZ_HPP

#include "comic.hpp"

class MyComicCBZ : public MyComic {
    Q_OBJECT

public :
    MyComicCBZ(QObject *parent = nullptr, QString filePath = "");

private :
    void loadComic(QString filePath);
};

#endif