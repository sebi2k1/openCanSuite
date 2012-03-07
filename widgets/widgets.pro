TEMPLATE = lib
CONFIG += staticlib
TARGET = widgets
QT += core \
      gui\
      xml
HEADERS += QRealtimePlotter.h
SOURCES += QRealtimePlotter.cc
LIBS += -lqwt -lQxtCore
INCLUDEPATH += /usr/include/qxt/QxtCore
