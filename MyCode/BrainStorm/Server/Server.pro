TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    thread.cpp \
    tcpsocket.cpp \
    mydatabase.cpp \
    myserver.cpp \
    tcpserver.cpp \
    user.cpp \
    tcptransfer.cpp


HEADERS += \
    thread.h \
    tcpsocket.h \
    mydatabase.h \
    myserver.h \
    tcpserver.h \
    mysignal.h \
    user.h \
    tcptransfer.h

LIBS += -L/usr/local/lib -levent -lpthread -lmysqlclient -ljson
