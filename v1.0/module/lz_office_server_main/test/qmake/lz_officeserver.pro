#-------------------------------------------------
#
# Project created by QtCreator 2013-11-23T21:18:48
#
#-------------------------------------------------

QT       += core gui network

INCLUDEPATH += ../../include \
    ../../include/gui \
    ../../../xmlloader/include \
    ../../../xmlloader/include\tinyxml \
    ../../../filetransport/include \
    ../../../compress/include \
    ../../../network_cs/include/server \
    ../../../datastructure/include

TARGET = lz_officeserver
TEMPLATE = app

SOURCES += ../main.cpp \
    ../../src/serverprogram.cpp \
    ../../src/setting_server.cpp \
    ../../../network_cs/src/server/multithreadtcpserver.cpp \
    ../../../network_cs/src/server/multithreadtcpserver_office.cpp \
    ../../../network_cs/src/server/serverthread.cpp \
    ../../../network_cs/src/server/serverthread_office.cpp \
    ../../src/gui/form.cpp \
    ../../../filetransport/src/filesender.cpp \
    ../../../filetransport/src/filelistsender.cpp \
    ../../../filetransport/src/filereceiver.cpp \
    ../../../filetransport/src/filereceiverserver.cpp \
    ../../../compress/src/file_decompress.cpp \
    ../../../datastructure/src/network_config_list.cpp \
    ../../../datastructure/src/slavemodel.cpp \
    ../../../datastructure/src/editing_list.cpp \
    ../../../datastructure/src/projectmodel.cpp \
    ../../../xmlloader/src/tinyxml/tinyxmlparser.cpp \
    ../../../xmlloader/src/tinyxml/tinyxmlerror.cpp \
    ../../../xmlloader/src/tinyxml/tinyxml.cpp \
    ../../../xmlloader/src/tinyxml/tinystr.cpp \
    ../../../xmlloader/src/xmlfileloader.cpp \
    ../../../xmlloader/src/xmlprojectfileloader.cpp

HEADERS  += \
    ../../include/serverprogram.h \
    ../../include/setting_server.h \
    ../../../network_cs/include/server/multithreadtcpserver.h \
    ../../../network_cs/include/server/multithreadtcpserver_office.h \
    ../../../network_cs/include/server/serverthread.h \
    ../../../network_cs/include/server/serverthread_office.h \
    ../../include/gui/form.h \
    ../../../filetransport/include/filesender.h \
    ../../../filetransport/include/filelistsender.h \
    ../../../filetransport/include/filereceiver.h \
    ../../../filetransport/include/filereceiverserver.h \
    ../../../filetransport/include/filename_filter.h \
    ../../../compress/include/file_decompress.h \
    ../../../datastructure/include/network_config_list.h \
    ../../../datastructure/include/slavemodel.h \
    ../../../datastructure/include/editing_list.h \
    ../../../datastructure/include/projectmodel.h \
    ../../../xmlloader/include/tinyxml/tinyxml.h \
    ../../../xmlloader/include/tinyxml/tinystr.h \
    ../../../xmlloader/include/xmlfileloader.h \
    ../../../xmlloader/include/xmlprojectfileloader.h

FORMS    += ../../src/gui/form.ui

