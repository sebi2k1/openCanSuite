TEMPLATE = lib
CONFIG += staticlib
TARGET = qcan
QT += core \
      xml
HEADERS += QCanSignals.h \
           QCanChannel.h
SOURCES += QCanSignals.cc \
           QCanChannel.cc
