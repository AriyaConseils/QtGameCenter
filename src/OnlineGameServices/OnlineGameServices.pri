QT       += core gui websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    $$PWD/src/QWisperInterface.cpp \
    $$PWD/src/QOnlineGameCenter.cpp \
    $$PWD/src/models/playermodel.cpp \
    $$PWD/src/models/roommodel.cpp \
    $$PWD/src/models/tableplayerproxymodel.cpp \
    $$PWD/src/models/treeitem.cpp

HEADERS += \
    $$PWD/src/QWisperInterface.h \
    $$PWD/src/QOnlineGameCenter.h \
    $$PWD/src/dataType/Notification.h \
    $$PWD/src/framework/helpers.h \
    $$PWD/src/models/playermodel.h \
    $$PWD/src/models/roommodel.h \
    $$PWD/src/models/tableplayerproxymodel.h \
    $$PWD/src/models/treeitem.h

INCLUDEPATH += $$PWD/src

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/lib
else: unix:!android: target.path = /opt/$${TARGET}/lib
!isEmpty(target.path): INSTALLS += target
