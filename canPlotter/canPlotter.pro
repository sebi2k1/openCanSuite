TEMPLATE = app
TARGET = canPlotter
QT += core \
    gui \
    xml
HEADERS += MainWindow.h
SOURCES += MainWindow.cc \
           main.cc
RESOURCES +=
LIBS += -L../qcan -lqcan -L../widgets -lwidgets -lqwt -lQxtCore 
INCLUDEPATH += /usr/include/qxt/QxtCore ../qcan ../widgets
