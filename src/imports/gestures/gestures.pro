TARGET  = gesturesqmlplugin
TARGETPATH = Qt/labs/gestures
include(../qimportbase.pri)

QT += declarative

SOURCES += qdeclarativegesturearea.cpp plugin.cpp
HEADERS += qdeclarativegesturearea_p.h

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

symbian:{
    load(data_caging_paths)
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
    
    importFiles.sources = gesturesqmlplugin.dll qmldir
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    
    DEPLOYMENT = importFiles
}

INSTALLS += target qmldir
