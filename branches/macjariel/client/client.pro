######################################################################
# Automatically generated by qmake (2.01a) Mon Sep 1 03:56:07 2008
######################################################################

TEMPLATE = app

# Input
HEADERS += gameloop.h \
 connecttoserverdialog.h \
 mainwindow.h \
 serverconnection.h \
 common.h \
 joingamedialog.h \
 logwidget.h \
 chatwidget.h \
 card.h \
 opponentwidget.h \
 game.h \
 cardwidget.h

FORMS += connecttoserverdialog.ui mainwindow.ui \
 joingamedialog.ui \
 logwidget.ui \
 chatwidget.ui \
 opponentwidget.ui
SOURCES += gameloop.cpp main.cpp \
 connecttoserverdialog.cpp \
 mainwindow.cpp \
 serverconnection.cpp \
 common.cpp \
 joingamedialog.cpp \
 logwidget.cpp \
 chatwidget.cpp \
 card.cpp \
 opponentwidget.cpp \
 game.cpp \
 cardwidget.cpp

QT += network \
 xml

RESOURCES += client.qrc

INCLUDEPATH += ../common

LIBS += ../common/libcommon.a

TARGETDEPS += ../common/libcommon.a

CONFIG -= release

CONFIG += debug

QMAKE_CXXFLAGS_DEBUG += -Wall
