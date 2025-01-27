#ifndef COMICCBZ_HPP
#define COMICCBZ_HPP

#include "comic.hpp"

class MyComicCBZ : public MyComic {
    Q_OBJECT

public :
    MyComicCBZ(QObject *parent = nullptr, QString filePath = "",
               void(*notifyPageLoaded)(QObject *parent, int pageNumber) = nullptr);

private :
    void loadNames(QString filePath);
    void loadPagesAsync(QString filePath);
    void simpleLoadComic(QString filePath);
};

#endif