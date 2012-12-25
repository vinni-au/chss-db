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
    messagedatareader.cpp \
    buffer/signature.cpp \
    buffer/heapfile.cpp \
    query/parser.cpp

HEADERS += \
    global.h \
    dbshell.hpp \
    db.hpp \
    buffer/buffermanager.h \
    query/query.hpp \
    query/queryprocessor.hpp \
    metadata.h \
    buffer/record.h \
    buffer/diskmanager.h \
    messagedatareader.hpp \
    buffer/buffermanager.h \
    buffer/signature.h \
    buffer/heapfile.h \
    query/parser.hpp

OTHER_FILES += \
    buffer/ABOUT \
    query/ABOUT \
    storage/ABOUT

