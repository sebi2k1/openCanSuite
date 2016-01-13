TEMPLATE = app
TARGET = canAnalyzer
QT += core \
    gui \
    widgets \
    xml
HEADERS += MainWindow.h 
SOURCES += main.cc MainWindow.cc
FORMS += mainwindow.ui
RESOURCES +=
LIBS += -L../qcan -lqcan
INCLUDEPATH += ../qcan
