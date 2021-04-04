TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++17
QMAKE_LFLAGS += -pthread

SOURCES += main.cpp \
    profiler.cpp

HEADERS += \
    deque.h \
    profiler.h \
    printer.h

