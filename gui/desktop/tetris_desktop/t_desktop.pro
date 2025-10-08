TEMPLATE = app
TARGET = tetris_desktop
INCLUDEPATH += .
QT += widgets
LIBS += -lncurses

HEADERS += tetris_qt.h ../../../brick_game/tetris/tetris.h
SOURCES += main.cpp tetris_qt.cpp ../../../brick_game/tetris/backend.c

# Добавьте следующие строки, если используете Q_OBJECT
CONFIG += c++20
CONFIG += qobject

LIBS += -lSDL2 -lSDL2_mixer
INCLUDEPATH += /usr/include/SDL2
