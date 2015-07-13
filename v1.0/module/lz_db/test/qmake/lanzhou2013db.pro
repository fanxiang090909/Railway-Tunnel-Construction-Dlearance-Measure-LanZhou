#-------------------------------------------------
#
# Project created by QtCreator 2013-08-01T14:28:55
#
#-------------------------------------------------

QT       += core gui sql

TARGET = lanzhou2013db
TEMPLATE = app

INCLUDEPATH += ..\..\..\..\include \
    ..\..\include \
    ..\..\..\datastructure\include \
    ..\..\..\xmlloader\include \
    ..\..\..\xmlloader\include\tinyxml \
    ..\..\..\calculate\include \

SOURCES += ..\main.cpp \
    ..\..\src\daoline.cpp \
    ..\..\src\daotunnel.cpp \
    ..\..\src\daostation.cpp \
    ..\..\src\daocurve.cpp \
    ..\..\src\daotask.cpp \
    ..\..\src\daoadmin.cpp \
    ..\..\src\daoclearanceoutput.cpp \
    ..\..\src\daooutputitem.cpp \
    ..\..\src\daotasktunnel.cpp \
    ..\..\src\dumpdb.cpp \
    ..\..\src\settingdb.cpp \
    ..\..\..\datastructure\src\tunneldatamodel.cpp \
    ..\..\..\datastructure\src\clearance_item.cpp \
    ..\..\..\datastructure\src\output_heights_list.cpp \
    ..\..\..\xmlloader\src\xmlfileloader.cpp \
    ..\..\..\xmlloader\src\xmlsynthesisheightsfileloader.cpp \
    ..\..\..\xmlloader\src\tinyxml\tinystr.cpp \
    ..\..\..\xmlloader\src\tinyxml\tinyxml.cpp \
    ..\..\..\xmlloader\src\tinyxml\tinyxmlerror.cpp \
    ..\..\..\xmlloader\src\tinyxml\tinyxmlparser.cpp \
    ..\..\..\calculate\src\LzSynth_n.cpp

HEADERS  += ..\..\..\include\lz_db.h \
    ..\..\include\connectdb.h \
    ..\..\include\daoline.h \
    ..\..\include\daotunnel.h \
    ..\..\include\daostation.h \
    ..\..\include\daocurve.h \
    ..\..\include\daotask.h \
    ..\..\include\daoadmin.h \
    ..\..\include\daoclearanceoutput.h \
    ..\..\include\daooutputitem.h \
    ..\..\include\daotasktunnel.h \
    ..\..\include\dumpdb.h \
    ..\..\include\settingdb.h \
    ..\..\..\datastructure\include\tunneldatamodel.h \
    ..\..\..\datastructure\include\clearance_item.h \
    ..\..\..\datastructure\include\output_heights_list.h \
    ..\..\..\xmlloader\include\xmlfileloader.h \
    ..\..\..\xmlloader\include\xmlsynthesisheightsfileloader.h \
    ..\..\..\xmlloader\include\tinyxml\tinystr.h \
    ..\..\..\xmlloader\include\tinyxml\tinyxml.h \
    ..\..\..\calculate\include\LzSynth_n.h
