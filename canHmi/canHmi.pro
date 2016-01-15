TEMPLATE = app
TARGET = canHmi
QT += core \
    gui \
    quick \
    widgets \
    xml
SOURCES += main.cc
RESOURCES +=
LIBS += -L../qcan -lqcan
INCLUDEPATH += ../qcan
