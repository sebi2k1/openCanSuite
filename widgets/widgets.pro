TEMPLATE = lib
CONFIG += staticlib
TARGET = widgets
QT += core \
      gui\
      widgets\
      xml
HEADERS += QRealtimePlotter.h
SOURCES += QRealtimePlotter.cc
LIBS += -lqwt-qt5
