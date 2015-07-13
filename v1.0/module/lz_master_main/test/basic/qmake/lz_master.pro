#-------------------------------------------------
#
# Project created by QtCreator 2013-09-05T14:57:13
#
#-------------------------------------------------

QT       += core network gui

TARGET = lz_master
#CONFIG   += console
#CONFIG   -= app_bundle

INCLUDEPATH += ../../../include \
    ../../../include/gui/test \
    ../../../../xmlloader/include \
    ../../../../xmlloader/include/tinyxml \
    ../../../../datastructure/include \
    ../../../../filetransport/include \
    ../../../../compress/include \
    ../../../../network_cs/include/server \

TEMPLATE = app

SOURCES += ../main.cpp \
    ../../../src/gui/test/form.cpp \
    ../../../src/status.cpp \
    ../../../src/setting_master.cpp \
    ../../../src/masterprogram.cpp \
    ../../../../network_cs/src/server/multithreadtcpserver.cpp \
    ../../../../network_cs/src/server/multithreadtcpserver_master.cpp \
    ../../../../network_cs/src/server/serverthread.cpp \
    ../../../../network_cs/src/server/serverthread_master.cpp \
    ../../../../xmlloader/src/xmlfileloader.cpp \
    ../../../../xmlloader/src/tinyxml/tinyxmlparser.cpp \
    ../../../../xmlloader/src/tinyxml/tinyxmlerror.cpp \
    ../../../../xmlloader/src/tinyxml/tinyxml.cpp \
    ../../../../xmlloader/src/tinyxml/tinystr.cpp \
    ../../../../xmlloader/src/xmlnetworkfileloader.cpp \
    ../../../../xmlloader/src/xmlsynthesisheightsfileloader.cpp \
    ../../../../xmlloader/src/xmltaskfileloader.cpp \
    ../../../../xmlloader/src/xmlrealtaskfileloader.cpp \
    ../../../../xmlloader/src/xmlcheckedtaskfileloader.cpp \
    ../../../../xmlloader/src/xmlprojectfileloader.cpp \
    ../../../../datastructure/src/slavemodel.cpp \
    ../../../../datastructure/src/network_config_list.cpp \
    ../../../../datastructure/src/output_heights_list.cpp \
    ../../../../datastructure/src/plantask_list.cpp \
    ../../../../datastructure/src/realtask_list.cpp \
    ../../../../datastructure/src/checkedtask.cpp \
    ../../../../datastructure/src/checkedtask_list.cpp \
    ../../../../datastructure/src/projectmodel.cpp \
    ../../../../filetransport/src/filesender.cpp \
    ../../../../filetransport/src/filelistsender.cpp \
    ../../../../filetransport/src/filereceiver.cpp \
    ../../../../filetransport/src/filereceiverserver.cpp \
    ../../../../compress/src/file_decompress.cpp \

HEADERS += \
    ../../../include/gui/test/form.h \
    ../../../include/status.h \
    ../../../include/setting_master.h \
    ../../../include/masterprogram.h \
    ../../../../network_cs/include/server/multithreadtcpserver.h \
    ../../../../network_cs/include/server/multithreadtcpserver_master.h \
    ../../../../network_cs/include/server/serverthread.h \
    ../../../../network_cs/include/server/serverthread_master.h \
    ../../../../xmlloader/include/xmlfileloader.h \
    ../../../../xmlloader/include/tinyxml/tinyxml.h \
    ../../../../xmlloader/include/tinyxml/tinystr.h \
    ../../../../xmlloader/include/xmlnetworkfileloader.h \
    ../../../../xmlloader/include/xmlsynthesisheightsfileloader.h \
    ../../../../xmlloader/include/xmltaskfileloader.h \
    ../../../../xmlloader/include/xmlrealtaskfileloader.h \
    ../../../../xmlloader/include/xmlcheckedtaskfileloader.h \
    ../../../../xmlloader/include/xmlprojectfileloader.h \
    ../../../../datastructure/include/slavemodel.h \
    ../../../../datastructure/include/network_config_list.h \
    ../../../../datastructure/include/output_heights_list.h \
    ../../../../datastructure/include/plantask.h \
    ../../../../datastructure/include/plantask_list.h \
    ../../../../datastructure/include/realtask.h \
    ../../../../datastructure/include/realtask_list.h \
    ../../../../datastructure/include/checkedtask.h \
    ../../../../datastructure/include/checkedtask_list.h \
    ../../../../datastructure/include/projectmodel.h \
    ../../../../filetransport/include/filesender.h \
    ../../../../filetransport/include/filelistsender.h \
    ../../../../filetransport/include/filereceiver.h \
    ../../../../filetransport/include/filereceiverserver.h \
    ../../../../filetransport/include/filename_filter.h \
    ../../../../compress/include/file_decompress.h

FORMS +=  ../../../src/gui/test/form.ui

RESOURCES += ../../../resource/gui/test/icons.qrc
