#-------------------------------------------------
#
# Project created by QtCreator 2014-01-19T14:57:13
#
#-------------------------------------------------

QT       += core network gui

TARGET = filesender
#CONFIG   += console
#CONFIG   -= app_bundle

INCLUDEPATH += ../../../include \
    ../../../include/gui/sender \

TEMPLATE = app

SOURCES += ../main.cpp \
    ../../../src/filesender.cpp \
    ../../../src/filelistsender.cpp \
    ../../../src/gui/sender/form.cpp \

HEADERS += \
    ../../../include/filesender.h \
    ../../../include/filelistsender.h \
    ../../../include/gui/sender/form.h \

FORMS +=  ../../../src/gui/sender/form.ui \
