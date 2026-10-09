QT += core gui widgets
TARGET = SudokuGame
TEMPLATE = app
CONFIG += c++11

SOURCES += \
        CSudoku.cpp \
        SudokuCell.cpp \
        SudokuGame.cpp \
        main.cpp

HEADERS += \
        CSudoku.h \
        DifficultyStrategy.h \
        SudokuCell.h \
        SudokuGame.h
