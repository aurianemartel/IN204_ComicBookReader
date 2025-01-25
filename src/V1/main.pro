QT += core gui widgets
CONFIG += c++11
HEADERS += viewer.hpp comic.hpp comicCBZ.hpp comicCBR.hpp gestionRAR.hpp
SOURCES += main.cpp viewer.cpp comic.cpp comicCBZ.cpp comicCBR.cpp gestionRAR.cpp
LIBS += -lquazip5 -lunrar