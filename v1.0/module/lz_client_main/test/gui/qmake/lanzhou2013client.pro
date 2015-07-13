#-------------------------------------------------
#
# Project created by QtCreator 2013-08-15T15:27:50
#
#-------------------------------------------------

QT       += core gui sql network opengl webkit

TARGET = lanzhou2013client

INCLUDEPATH += D:/Boost/include/boost-1_55 \
    ../../../../include \
    ../../../include \
    ../../../include/excel \
    ../../../include/gui \
    ../../../../xmlloader/include \
    ../../../../xmlloader/include/tinyxml \
    ../../../../lz_db/include \
    ../../../../lz_db/include/gui \
    ../../../../datastructure/include \
    ../../../../filetransport/include \
    ../../../../network_cs/include/client \
    ../../../../compress/include \
    ../../../../filemanagers/include \
    ../../../../lz_exception/include \
    ../../../../calculate/include \
    ../../../../lz_output/include \
    ../../../../lz_output/include/gui \
    ../../../../lz_logger/include \
    ../../../../lz_master_main/include/gui \

LIBS += D:/Boost/lib/x86vc10/libboost_graph-vc100-mt-1_55.lib \
D:/Boost/lib/x86vc10/libboost_iostreams-vc100-1_55.lib \
D:/Boost/lib/x86vc10/libboost_locale-vc100-mt-1_55.lib \
D:/Boost/lib/x86vc10/libboost_date_time-vc100-mt-gd-1_55.lib \

CONFIG  += qaxcontainer

SOURCES += ../main.cpp \
    ../../../src/setting_client.cpp \
    ../../../src/clientprogram.cpp \
    ../../../../network_cs/src/client/client.cpp \
    ../../../src/gui/login.cpp \
    ../../../src/gui/about.cpp \
    ../../../src/gui/tabwidgetmanager.cpp \
    ../../../src/gui/DrawImage.cpp \
    ../../../src/gui/widgetone.cpp \
    ../../../src/gui/windowmanager.cpp \
    ../../../src/gui/widgetheader.cpp \
    ../../../src/gui/mainwidget.cpp \
    ../../../src/gui/correct_clearance.cpp \
    ../../../src/gui/select_tunnel.cpp \
    ../../../src/gui/synthesis_correct.cpp \
    ../../../src/gui/synthesis_tunnel.cpp \
    ../../../src/gui/multiframecorrect.cpp \
    ../../../../lz_db/src/gui/manage_tunnel.cpp \
    ../../../../lz_db/src/gui/manage_line.cpp \
    ../../../../lz_db/src/gui/backup_db.cpp \
    ../../../../lz_db/src/gui/output_clearance.cpp \
    ../../../../lz_db/src/gui/select_historical_tunnel.cpp \
    ../../../../lz_db/src/gui/input_bridge_clearance.cpp \
    ../../../../xmlloader/src/tinyxml/tinyxmlparser.cpp \
    ../../../../xmlloader/src/tinyxml/tinyxmlerror.cpp \
    ../../../../xmlloader/src/tinyxml/tinyxml.cpp \
    ../../../../xmlloader/src/tinyxml/tinystr.cpp \
    ../../../../xmlloader/src/xmlfileloader.cpp \
    ../../../../xmlloader/src/xmlnetworkfileloader.cpp \
    ../../../../xmlloader/src/xmlsynthesisheightsfileloader.cpp \
    ../../../../xmlloader/src/xmltaskfileloader.cpp \
    ../../../../xmlloader/src/xmlrealtaskfileloader.cpp \
    ../../../../xmlloader/src/xmlcheckedtaskfileloader.cpp \
    ../../../../xmlloader/src/xmlprojectfileloader.cpp \
    ../../../../datastructure/src/slavemodel.cpp \
    ../../../../datastructure/src/network_config_list.cpp \
    ../../../../datastructure/src/clearance_edit_record.cpp \
    ../../../../datastructure/src/clearance_item.cpp \
    ../../../../datastructure/src/clearance_tunnel.cpp \
    ../../../../datastructure/src/clearance_tunnels.cpp \
    ../../../../datastructure/src/output_heights_list.cpp \
    ../../../../datastructure/src/plantask_list.cpp \
    ../../../../datastructure/src/realtask_list.cpp \
    ../../../../datastructure/src/checkedtask.cpp \
    ../../../../datastructure/src/checkedtask_list.cpp \
    ../../../../datastructure/src/tunneldatamodel.cpp \
    ../../../../datastructure/src/projectmodel.cpp \
    ../../../../lz_db/src/daoline.cpp \
    ../../../../lz_db/src/daotunnel.cpp \
    ../../../../lz_db/src/daostation.cpp \
    ../../../../lz_db/src/daocurve.cpp \
    ../../../../lz_db/src/daotask.cpp \
    ../../../../lz_db/src/daoadmin.cpp \
    ../../../../lz_db/src/daoclearanceoutput.cpp \
    ../../../../lz_db/src/daooutputitem.cpp \
    ../../../../lz_db/src/daotasktunnel.cpp \
    ../../../../lz_db/src/dumpdb.cpp \
    ../../../../lz_db/src/settingdb.cpp \
    ../../../../filetransport/src/filereceiver.cpp \
    ../../../../filetransport/src/filereceiverserver.cpp \
    ../../../../filetransport/src/filesender.cpp \
    ../../../../filetransport/src/filelistsender.cpp \
    ../../../../compress/src/file_decompress.cpp \
    ../../../../datastructure/src/clearance_item.cpp \
    ../../../../calculate/src/LzSynth.cpp \
    ../../../../calculate/src/LzSynth_n.cpp \
    ../../../../filemanagers/src/LzSerialStorageBase.cpp \
    ../../../../lz_output/src/lz_output_access.cpp \
    ../../../../lz_output/src/lz_html_output.cpp \
    ../../../../lz_output/src/lz_excel_output.cpp \
    ../../../../lz_output/src/excelengine.cpp \
    ../../../../lz_output/src/gui/textedit.cpp \
    ../../../../lz_logger/src/lz_logger.cpp \
    ../../../../lz_master_main/src/gui/imageviewer.cpp \
    ../../../../lz_master_main/src/gui/imageviewer_client.cpp \

HEADERS  += \
    ../../../include/setting_client.h \
    ../../../include/clientprogram.h \
    ../../../../network_cs/include/client/client.h \
    ../../../include/gui/login.h \
    ../../../include/gui/about.h \
    ../../../include/gui/tabwidgetmanager.h \
    ../../../include/gui/DrawImage.h \
    ../../../include/gui/widgetone.h \
    ../../../include/gui/windowmanager.h \
    ../../../include/gui/widgetheader.h \
    ../../../include/gui/mainwidget.h \
    ../../../include/gui/correct_clearance.h \
    ../../../include/gui/select_tunnel.h \
    ../../../include/gui/synthesis_tunnel.h \
    ../../../include/gui/synthesis_correct.h \
    ../../../include/gui/multiframecorrect.h \
    ../../../../lz_db/include/gui/manage_tunnel.h \
    ../../../../lz_db/include/gui/manage_line.h \
    ../../../../lz_db/include/gui/backup_db.h \
    ../../../../lz_db/include/gui/output_clearance.h \
    ../../../../lz_db/include/gui/select_historical_tunnel.h \
    ../../../../lz_db/include/gui/input_bridge_clearance.h \
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
    ../../../../datastructure/include/clearance_edit_record.h \
    ../../../../datastructure/include/clearance_item.h \
    ../../../../datastructure/include/clearance_tunnel.h \
    ../../../../datastructure/include/clearance_tunnels.h \
    ../../../../datastructure/include/output_heights_list.h \
    ../../../../datastructure/include/plantask.h \
    ../../../../datastructure/include/plantask_list.h \
    ../../../../datastructure/include/realtask.h \
    ../../../../datastructure/include/realtask_list.h \
    ../../../../datastructure/include/checkedtask.h \
    ../../../../datastructure/include/checkedtask_list.h \
    ../../../../datastructure/include/tunneldatamodel.h \
    ../../../../datastructure/include/projectmodel.h \
    ../../../../lz_db/include/daoline.h \
    ../../../../lz_db/include/daotunnel.h \
    ../../../../lz_db/include/daostation.h \
    ../../../../lz_db/include/daocurve.h \
    ../../../../lz_db/include/daotask.h \
    ../../../../lz_db/include/daoadmin.h \
    ../../../../lz_db/include/daoclearanceoutput.h \
    ../../../../lz_db/include/daooutputitem.h \
    ../../../../lz_db/include/daotasktunnel.h \
    ../../../../lz_db/include/dumpdb.h \
    ../../../../lz_db/include/settingdb.h \
    ../../../../lz_db/include/connectdb.h \
    ../../../../filetransport/include/filereceiver.h \
    ../../../../filetransport/include/filereceiverserver.h \
    ../../../../filetransport/include/filesender.h \
    ../../../../filetransport/include/filelistsender.h \
    ../../../../compress/include/file_decompress.h \
    ../../../../datastructure/include/clearance_item.h \
    ../../../../lz_exception/include/LzException.h \
    ../../../../calculate/include/LzSynth.h \
    ../../../../calculate/include/LzSynth_n.h \
    ../../../../filemanagers/include/LzSerialStorageBase.h \
    ../../../../filemanagers/include/LzSerialStorageSynthesis.h \
    ../../../../lz_output/include/lz_output_access.h \
    ../../../../lz_output/include/lz_html_output.h \
    ../../../../lz_output/include/lz_excel_output.h \
    ../../../../lz_output/include/excelengine.h \
    ../../../../lz_output/include/gui/textedit.h \
    ../../../../lz_logger/include/lz_logger.h \
    ../../../../lz_master_main/include/gui/imageviewer.h \

FORMS    += \
    ../../../src/gui/login.ui \
    ../../../src/gui/about.ui \
    ../../../src/gui/tabwidgetmanager.ui \
    ../../../src/gui/DrawImage.ui \
    ../../../src/gui/widgetone.ui \
    ../../../src/gui/widgetheader.ui \
    ../../../src/gui/mainwidget.ui \
    ../../../src/gui/correct_clearance.ui \
    ../../../src/gui/select_tunnel.ui \
    ../../../src/gui/synthesis_tunnel.ui \
    ../../../src/gui/synthesis_correct.ui \
    ../../../src/gui/multiframecorrect.ui \
    ../../../../lz_db/src/gui/manage_tunnel.ui \
    ../../../../lz_db/src/gui/manage_line.ui \
    ../../../../lz_db/src/gui/backup_db.ui \
    ../../../../lz_db/src/gui/output_clearance.ui \
    ../../../../lz_db/src/gui/select_historical_tunnel.ui \
    ../../../../lz_db/src/gui/input_bridge_clearance.ui \
    ../../../../lz_master_main/src/gui/imageviewer.ui \

RESOURCES += \
    ../../../resource/icons.qrc \
    ../../../resource/player.qrc \
    ../../../../lz_output/resource/textedit.qrc \
