#ifndef GESTIONRAR_HPP
#define GESTIONRAR_HPP

#include "comicCBR.hpp"

HANDLE openRAR(QString filePath);

void getSortedFileNamesRAR(QString filePath, QStringList& zipFiles);

HANDLE getImageFromNameCBR(HANDLE hRAR, QString filePath, QString fileName, QByteArray& fileData);

#endif