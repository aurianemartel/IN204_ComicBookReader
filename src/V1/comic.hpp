#ifndef COMIC_HPP
#define COMIC_HPP

#include <QObject>
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
    QStringList names;
    void (*notifyPageLoaded)(QObject *parent, int pageNumber) = nullptr;

    virtual void loadComic(QString filePath);
    virtual void loadNames(QString filePath) = 0;
    virtual void loadPagesAsync(QString filePath) = 0;
    
    void useNotifyLoading(int pageNumber);

};

#endif




