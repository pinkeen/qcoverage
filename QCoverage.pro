TARGET = qcoverage
HEADERS += \
    mainwindow.h \
    graphscenememento.h \
    graphscene.h \
    graphnodetext.h \
    graphnode.h \
    graphedge.h \
    graphproxy.h \
    algorithmnode.h \
    algorithmpath.h \
    abstractalgorithm.h \
    nodesalgorithm.h \
    edgesalgorithm.h \
    edgepairalgorithm.h \
    simplepathsalgorithm.h \
    primepathsalgorithm.h

SOURCES += \
    mainwindow.cpp \
    main.cpp \
    graphscenememento.cpp \
    graphscene.cpp \
    graphnodetext.cpp \
    graphnode.cpp \
    graphedge.cpp \
    graphproxy.cpp \
    algorithmnode.cpp \
    algorithmpath.cpp \
    abstractalgorithm.cpp \
    nodesalgorithm.cpp \
    edgesalgorithm.cpp \
    edgepairalgorithm.cpp \
    simplepathsalgorithm.cpp \
    primepathsalgorithm.cpp

FORMS += \
    mainwindow.ui

RESOURCES += \
    QCoverage.qrc
