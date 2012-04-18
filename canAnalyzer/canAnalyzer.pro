TEMPLATE = app
TARGET = canAnalyzer
QT += core \
    gui \
    xml
HEADERS += MainWindow.h 
SOURCES += main.cc MainWindow.cc
FORMS += mainwindow.ui
RESOURCES +=
LIBS += -L../qcan -lqcan -L../widgets -lwidgets -lqwt -lQxtCore 
INCLUDEPATH += /usr/include/qxt/QxtCore ../qcan ../widgets
