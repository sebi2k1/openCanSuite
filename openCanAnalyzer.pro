TEMPLATE = app
TARGET = openCanAnalyzer
QT += core \
    gui
HEADERS += qcan/QCanSignals.h \
    qcan/QCanChannel.h \
    MainWindow.h
SOURCES += qcan/QCanSignals.cc \
    qcan/QCanChannel.cc \
    MainWindow.cc \
    main.cc
RESOURCES += 
