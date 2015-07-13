#-------------------------------------------------
#
# Project created by QtCreator 2013-08-15T15:27:50
#
#-------------------------------------------------

QT       += core gui sql network opengl

TARGET = lanzhou2013client

INCLUDEPATH += ../../../include \
    ../../../include/gui/test \
    ../../../../xmlloader/include \
    ../../../../xmlloader/include/tinyxml \
    ../../../../datastructure/include \
    ../../../../network_cs/include/client \
    ../../../../filetransport/include \
    ../../../../compress/include \
    ../../../../lz_db/include \
    ../../../../calculate/include \


CONFIG  += qaxcontainer

SOURCES += ../main.cpp \
    ../../../src/setting_client.cpp \
    ../../../src/clientprogram.cpp \
    ../../../../network_cs/src/client/client.cpp \
    ../../../src/gui/test/form.cpp \
    ../../../../xmlloader/src/xmlfileloader.cpp \
    ../../../../xmlloader/src/xmlprojectfileloader.cpp \
    ../../../../xmlloader/src/tinyxml/tinyxmlparser.cpp \
    ../../../../xmlloader/src/tinyxml/tinyxmlerror.cpp \
    ../../../../xmlloader/src/tinyxml/tinyxml.cpp \
    ../../../../xmlloader/src/tinyxml/tinystr.cpp \
    ../../../../datastructure/src/slavemodel.cpp \
    ../../../../datastructure/src/projectmodel.cpp \
    ../../../../datastructure/src/network_config_list.cpp \
    ../../../../datastructure/src/output_heights_list.cpp \
    ../../../../datastructure/src/tunneldatamodel.cpp \
    ../../../../datastructure/src/clearance_item.cpp \
    ../../../../datastructure/src/clearance_tunnel.cpp \
    ../../../../datastructure/src/clearance_tunnels.cpp \
    ../../../../lz_db/src/daotunnel.cpp \
    ../../../../lz_db/src/daocurve.cpp \
    ../../../../lz_db/src/daoline.cpp \
    ../../../../lz_db/src/daostation.cpp \
    ../../../../lz_db/src/daotask.cpp \
    ../../../../lz_db/src/daoclearanceoutput.cpp \
    ../../../../lz_db/src/daooutputitem.cpp \
    ../../../../lz_db/src/daotasktunnel.cpp \
    ../../../../filetransport/src/filesender.cpp \
    ../../../../filetransport/src/filelistsender.cpp \
    ../../../../filetransport/src/filereceiver.cpp \
    ../../../../filetransport/src/filereceiverserver.cpp \
    ../../../../compress/src/file_decompress.cpp \
    ../../../../calculate/src/LzSynth_n.cpp \

HEADERS  += \
    ../../../include/setting_client.h \
    ../../../include/clientprogram.h \
    ../../../../network_cs/include/client/client.h \
    ../../../include/gui/test/form.h \
    ../../../../xmlloader/include/xmlfileloader.h \
    ../../../../xmlloader/include/xmlprojectfileloader.h \
    ../../../../xmlloader/include/tinyxml/tinyxml.h \
    ../../../../xmlloader/include/tinyxml/tinystr.h \
    ../../../../datastructure/include/projectmodel.h \
    ../../../../datastructure/include/slavemodel.h \
    ../../../../datastructure/include/network_config_list.h \
    ../../../../datastructure/include/output_heights_list.h \
    ../../../../datastructure/include/tunneldatamodel.h \
    ../../../../datastructure/include/clearance_item.h \
    ../../../../datastructure/include/clearance_tunnel.h \
    ../../../../datastructure/include/clearance_tunnels.h \
    ../../../../lz_db/include/daotunnel.h \
    ../../../../lz_db/include/daocurve.h \
    ../../../../lz_db/include/daoline.h \
    ../../../../lz_db/include/daostation.h \
    ../../../../lz_db/include/daotask.h \
    ../../../../lz_db/include/daoclearanceoutput.h \
    ../../../../lz_db/include/daooutputitem.h \
    ../../../../lz_db/include/daotasktunnel.h \
    ../../../../filetransport/include/filesender.h \
    ../../../../filetransport/include/filelistsender.h \
    ../../../../filetransport/include/filename_filter.h \
    ../../../../filetransport/include/filereceiver.h \
    ../../../../filetransport/include/filereceiverserver.h \
    ../../../../compress/include/file_decompress.h \
    ../../../../calculate/include/LzSynth_n.h \

FORMS    += ../../../src/gui/test/form.ui \
