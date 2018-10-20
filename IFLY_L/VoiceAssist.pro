#-------------------------------------------------
#
# Project created by QtCreator 2017-03-11T13:09:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += multimedia

TARGET = LabAssist
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp\
        mainwindow.cpp \
        micaudio.cpp \
    iflyvoice.cpp

HEADERS  += mainwindow.h \
        micaudio.h \
    iflyvoice.h \
    msp_cmn.h \
    msp_errors.h \
    msp_types.h \
    qisr.h \
    qtts.h

FORMS    += mainwindow.ui

# 添加讯飞语音链接�LIBS += "$$PWD/msc.dll"

LIBS += -L$$PWD/./ -lwinmm
