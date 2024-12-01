QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
include(src/OnlineGameServices/OnlineGameServices.pri)
include(src/InteractiveChat/InteractiveChat.pri)
include(src/GameCenter/GameCenter.pri)



# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/fireworksanimation.cpp \
    src/playerroom.cpp \
    src/playerstatistics.cpp \
    src/signupdialog.cpp \
    src/widget/statuswidget.cpp \
    src/Delegates/playerdelegate.cpp

HEADERS += \
    src/fireworksanimation.h \
    src/mainwindow.h \
    src/playerroom.h \
    src/playerstatistics.h \
    src/signupdialog.h \
    src/widget/statuswidget.h \
    src/Delegates/playerdelegate.h

FORMS += \
    src/mainwindow.ui \
    src/playerroom.ui \
    src/playerstatistics.ui \
    src/signupdialog.ui

INCLUDEPATH += $$PWD/src

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    src/ressources/ressources.qrc
