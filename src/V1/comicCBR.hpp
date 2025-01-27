#ifndef COMICCBR_HPP
#define COMICCBR_HPP

#include "comic.hpp"

class MyComicCBR : public MyComic {
    Q_OBJECT

public :
    MyComicCBR(QObject *parent = nullptr, QString filePath = "",
               void(*notifyPageLoaded)(QObject *parent, int pageNumber) = nullptr);

private :
    void loadNames(QString filePath);
    void loadPagesAsync(QString filePath);
    void simpleLoadComic(QString filePath);
};

#endif