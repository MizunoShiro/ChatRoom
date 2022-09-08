#-------------------------------------------------
#
# Project created by QtCreator 2022-08-15T18:35:21
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    register.cpp \
    communicate.cpp \
    login.cpp \
    brainstorm.cpp \
    root.cpp \
    imagetransfer.cpp

HEADERS += \
    register.h \
    communicate.h \
    login.h \
    mysignal.h \
    brainstorm.h \
    root.h \
    imagetransfer.h

FORMS += \
    register.ui \
    login.ui \
    brainstorm.ui \
    root.ui \
    imagetransfer.ui