#-------------------------------------------------
#
# Project created by QtCreator 2014-01-19T14:57:13
#
#-------------------------------------------------

QT       += core network gui

TARGET = filereceiver
#CONFIG   += console
#CONFIG   -= app_bundle

INCLUDEPATH += ../../../include \
    ../../../../compress/include \

TEMPLATE = app

SOURCES += ../main.cpp \
    ../../../src/filereceiver.cpp \
    ../../../src/filereceiverserver.cpp \
    ../../../../compress/src/file_decompress.cpp \

HEADERS += \
    ../../../include/filename_filter.h \
    ../../../include/filereceiver.h \
    ../../../include/filereceiverserver.h \
    ../../../../compress/include/file_decompress.h \

