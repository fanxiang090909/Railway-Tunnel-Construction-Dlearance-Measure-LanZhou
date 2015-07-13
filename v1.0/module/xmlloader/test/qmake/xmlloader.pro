#-------------------------------------------------
#
# Project created by QtCreator 2013-11-04T19:11:12
#
#-------------------------------------------------

QT       += core gui

TARGET = xmlloader
TEMPLATE = app

INCLUDEPATH += ../../include \
    ../../include/tinyxml \
    ../../../datastructure/include \
    ../../../../include \
    ../../../lz_master_main/include

SOURCES += \
    ../main.cpp \
    ../../src/xmlfileloader.cpp \
    ../../src/tinyxml/tinyxmlparser.cpp \
    ../../src/tinyxml/tinyxmlerror.cpp \
    ../../src/tinyxml/tinyxml.cpp \
    ../../src/tinyxml/tinystr.cpp \
    ../../src/xmlnetworkfileloader.cpp \
    ../../src/xmlsynthesisheightsfileloader.cpp \
    ../../src/xmltaskfileloader.cpp \
    ../../src/xmlrealtaskfileloader.cpp \
    ../../src/xmlcheckedtaskfileloader.cpp \
    ../../src/xmlprojectfileloader.cpp \
    ../../src/xmleditinglistfileloader.cpp \
    ../../../datastructure/src/slavemodel.cpp \
    ../../../datastructure/src/network_config_list.cpp \
    ../../../datastructure/src/output_heights_list.cpp \
    ../../../datastructure/src/plantask_list.cpp \
    ../../../datastructure/src/realtask_list.cpp \
    ../../../datastructure/src/checkedtask.cpp \
    ../../../datastructure/src/checkedtask_list.cpp \
    ../../../datastructure/src/editing_list.cpp \
    ../../../datastructure/src/projectmodel.cpp

HEADERS  += \
    ../../include/xmlfileloader.h \
    ../../include/tinyxml/tinyxml.h \
    ../../include/tinyxml/tinystr.h \
    ../../include/xmlnetworkfileloader.h \
    ../../include/xmlsynthesisheightsfileloader.h \
    ../../include/xmltaskfileloader.h \
    ../../include/xmlrealtaskfileloader.h \
    ../../include/xmlcheckedtaskfileloader.h \
    ../../include/xmlprojectfileloader.h \
    ../../include/xmleditinglistfileloader.h \
    ../../../datastructure/include/slavemodel.h \
    ../../../datastructure/include/network_config_list.h \
    ../../../datastructure/include/output_heights_list.h \
    ../../../datastructure/include/plantask.h \
    ../../../datastructure/include/plantask_list.h \
    ../../../datastructure/include/realtask.h \
    ../../../datastructure/include/realtask_list.h \
    ../../../datastructure/include/checkedtask.h \
    ../../../datastructure/include/checkedtask_list.h \
    ../../../datastructure/include/editing_list.h \
    ../../../datastructure/include/projectmodel.h
