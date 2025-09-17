QT       += core gui widgets  # Modules Qt utilisés
CONFIG   += c++17            # Standard C++17
TARGET   = GameCore          # Nom du binaire final
TEMPLATE = app               # Type de projet : application

# ---------------------
# Sources CPP
# ---------------------
SOURCES += ../src/cpp/main.cpp \
           ../src/cpp/Emulator.cpp \
           ../src/cpp/EmulatorWidget.cpp \
           ../src/cpp/Carousel.cpp # Ajouté

# ---------------------
# Headers H
# ---------------------
HEADERS += ../src/headers/Emulator.h \
           ../src/headers/EmulatorWidget.h \
           ../src/headers/Constants.h \
           ../src/headers/Carousel.h # Ajouté
