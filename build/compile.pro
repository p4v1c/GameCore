QT += core gui widgets
QT += core gui widgets
CONFIG += c++17
TARGET = GameCore
TEMPLATE = app
LIBS += -lSDL2
INCLUDEPATH += /usr/include/SDL2
INCLUDEPATH += $$PWD/../headers

# ---------------------
# Sources CPP
# ---------------------
SOURCES += ../src/cpp/main.cpp \
           ../src/cpp/Emulator.cpp \
           ../src/cpp/EmulatorWidget.cpp \
           ../src/cpp/ControllerManager.cpp \
           ../src/cpp/Carousel.cpp \
           ../src/cpp/GameListWidget.cpp \
           ../src/cpp/PowerMenuWidget.cpp \
           ../src/cpp/EmulatorConfig.cpp

# ---------------------
# Headers H
# ---------------------
HEADERS += ../src/headers/Emulator.h \
           ../src/headers/EmulatorWidget.h \
           ../src/headers/ControllerManager.h \
           ../src/headers/Constants.h \
           ../src/headers/Carousel.h \
           ../src/headers/GameListWidget.h \
           ../src/headers/PowerMenuWidget.h \
           ../src/headers/EmulatorConfig.h
