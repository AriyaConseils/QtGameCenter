QT       += core gui websockets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    $$PWD/src/bots/AbstractBot/AbstractBot.cpp  \
    $$PWD/src/games/AbstractGame/AbstractTableGame.cpp \
    $$PWD/src/games/TicTacToeGames.cpp \
    $$PWD/src/games/checkersgame.cpp \
    $$PWD/src/games/gogames.cpp \
	
HEADERS += \
    $$PWD/src/GameCatalogModel.h \
    $$PWD/src/bots/AbstractBot/AbstractBot.h \
    $$PWD/src/bots/RandomBot.h \
    $$PWD/src/framework/types/QCircularList.h \
    $$PWD/src/framework/types/QMatrix.h \
    $$PWD/src/games/AbstractGame/AbstractStrategyRulesGame.h \
    $$PWD/src/games/AbstractGame/AbstractTableGame.h \
    $$PWD/src/games/AbstractGame/Player.h \
    $$PWD/src/games/CheckersRules.h \
    $$PWD/src/games/GoRules.h \
    $$PWD/src/games/TicTacToeGames.h \
    $$PWD/src/games/TicTacToeRules.h \
    $$PWD/src/games/checkersgame.h \
    $$PWD/src/games/gogames.h



INCLUDEPATH += $$PWD/src

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/lib
else: unix:!android: target.path = /opt/$${TARGET}/lib
!isEmpty(target.path): INSTALLS += target
