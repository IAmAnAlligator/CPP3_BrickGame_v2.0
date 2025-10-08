TEMPLATE = app
TARGET = snake_desktop
INCLUDEPATH += .
QT += widgets
LIBS += -lncurses

HEADERS += snake_qt.h ../../../brick_game/snake/snake.h ../../../brick_game/snake/model/model.h ../../../brick_game/snake/controller/controller.h 
SOURCES += main.cpp snake_qt.cpp ../../../brick_game/snake/model/model.cpp ../../../brick_game/snake/controller/controller.cpp

# Добавьте следующие строки, если используете Q_OBJECT
CONFIG += c++20
CONFIG += qobject

RESOURCES += img.qrc