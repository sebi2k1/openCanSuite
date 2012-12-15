TEMPLATE = app
TARGET = canHmi
QT += core \
    gui \
    declarative \
    xml
SOURCES += main.cc
RESOURCES +=
LIBS += -L../qcan -lqcan -lQxtCore 
INCLUDEPATH += /usr/include/qxt/QxtCore ../qcan
