TEMPLATE = app
TARGET = openCanAnalyzer
QT += core \
    gui \
    xml
HEADERS += QRealtimePlotter.h \
    qcan/QCanSignals.h \
    qcan/QCanChannel.h \
    MainWindow.h
SOURCES += QRealtimePlotter.cc \
    qcan/QCanSignals.cc \
    qcan/QCanChannel.cc \
    MainWindow.cc \
    main.cc
RESOURCES += 
LIBS += -lqwt