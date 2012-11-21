#-------------------------------------------------
#
# Project created by QtCreator 2012-11-21T22:20:40
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = FatConsole
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
CONFIG   += console exceptions

unix {
LIBS += -L/usr/local/lib -lreadline.6.2 -lhistory.6.2
}

macx {
    QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
    QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
}


SOURCES += main.cpp \
	 ../unix/diskio.c \
    ff.c \
    console.cpp \
    commandinterpreter.cpp \
    qsignalcatcher.cpp \
    helpers.cpp \
    fatcommandinterpreter.cpp
         

HEADERS += \
    integer.h \
    ffconf.h \
    ff.h \
    diskio.h \
    console.h \
    commandinterpreter.h \
    qsignalcatcher.h \
    helpers.h \
    fatcommandinterpreter.h
