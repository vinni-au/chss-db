TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp \
    dbshell.cpp \
    db.cpp \
    buffer/buffermanager.cpp \
    global.cpp \
    query/query.cpp \
    query/queryprocessor.cpp \
    buffer/record.cpp \
    buffer/diskmanager.cpp \
    messagedatareader.cpp

HEADERS += \
    global.h \
    dbshell.hpp \
    db.hpp \
    buffer/buffermanager.hpp \
    query/query.hpp \
    query/queryprocessor.hpp \
    metadata.h \
    buffer/record.h \
    buffer/diskmanager.h \
    messagedatareader.hpp

OTHER_FILES += \
    buffer/ABOUT \
    query/ABOUT \
    storage/ABOUT

