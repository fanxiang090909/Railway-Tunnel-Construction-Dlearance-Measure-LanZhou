#-------------------------------------------------
#
# Project created by QtCreator 2013-12-03T16:16:08
#
#-------------------------------------------------

QT       += core gui network

TARGET = CViceProgram
TEMPLATE = app

INCLUDEPATH +=  \
    ../../include \
    ../../include/gui \
    ../../../xmlloader/include \
    ../../../xmlloader/include/tinyxml \
    ../../../datastructure/include \
    ../../../lz_exception/include \
    ../../../network_cs/include/client \
    ../../../filetransport/include \
    ../../../compress/include \

SOURCES += ../main.cpp \
    ../../src/gui/form.cpp \
    ../../src/gui/formmanager.cpp \
    ../../src/setting_slave.cpp \
    ../../src/cviceprogram.cpp \
    ../../../network_cs/src/client/client.cpp \
    ../../../xmlloader/src/xmlfileloader.cpp \
    ../../../xmlloader/src/tinyxml/tinyxmlparser.cpp \
    ../../../xmlloader/src/tinyxml/tinyxmlerror.cpp \
    ../../../xmlloader/src/tinyxml/tinyxml.cpp \
    ../../../xmlloader/src/tinyxml/tinystr.cpp \
    ../../../xmlloader/src/xmlnetworkfileloader.cpp \
    ../../../xmlloader/src/xmltaskfileloader.cpp \
    ../../../datastructure/src/checkedtask_list.cpp \
    ../../../datastructure/src/plantask_list.cpp \
    ../../../datastructure/src/slavemodel.cpp \
    ../../../datastructure/src/camerasnid.cpp \
    ../../../filetransport/src/filesender.cpp \
    ../../../filetransport/src/filereceiver.cpp \
    ../../../compress/src/file_decompress.cpp \

HEADERS  += \
    ../../include/gui/form.h \
    ../../include/gui/formmanager.h \
    ../../include/setting_slave.h \
    ../../include/cviceprogram.h \
    ../../../network_cs/src/client/client.h \
    ../../../xmlloader/include/xmlfileloader.h \
    ../../../xmlloader/include/tinyxml/tinyxml.h \
    ../../../xmlloader/include/tinyxml/tinystr.h \
    ../../../xmlloader/include/xmlnetworkfileloader.h \
    ../../../xmlloader/include/xmltaskfileloader.h \
    ../../../datastructure/include/checkedtask_list.h \
    ../../../datastructure/include/plantask_list.h \
    ../../../datastructure/include/slavemodel.h \
    ../../../datastructure/include/camerasnid.h \
    ../../../datastructure/include/output_heights_list.h \
    ../../../lz_exception/include/LzException.h \
    ../../../filetransport/include/filesender.h \
    ../../../filetransport/include/filereceiver.h \
    ../../../compress/include/file_decompress.h \

FORMS    += \
    ../../src/gui/form.ui
