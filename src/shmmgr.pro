QT = core gui widgets sql printsupport
TARGET = SHM-Manager
TEMPLATE = app
RC_FILE = shmmgr.rc
DEFINES += QT_DEPRECATED_WARNINGS
DESTDIR = $$PWD/../dist

SOURCES += \
    main.cpp \
    shmmanager.cpp \
    shmeditor.cpp \
    db.cpp \
    logindialog.cpp \
    changepassworddialog.cpp

HEADERS += \
    shmmanager.h \
    shmeditor.h \
    db.h \
    logindialog.h \
    changepassworddialog.h \
    db.config.h

FORMS += \
    shmmanager.ui \
    shmeditor.ui \
    logindialog.ui \
    changepassworddialog.ui

RESOURCES += \
    resources.qrc

DISTFILES +=
