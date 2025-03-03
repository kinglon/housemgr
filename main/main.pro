QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Utility/DumpUtil.cpp \
    Utility/IcrCriticalSection.cpp \
    Utility/ImCharset.cpp \
    Utility/ImPath.cpp \
    Utility/LogBuffer.cpp \
    Utility/LogUtil.cpp \
    downloadimagecontroller.cpp \
    housedialog.cpp \
    househttpclient.cpp \
    httpclientbase.cpp \
    imagedialog.cpp \
    loginwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    modifypassworddialog.cpp \
    myprogressdialog.cpp \
    pathutil.cpp \
    settingmanager.cpp \
    uiutil.cpp \
    uploadimagecontroller.cpp

HEADERS += \
    Utility/DumpUtil.h \
    Utility/IcrCriticalSection.h \
    Utility/ImCharset.h \
    Utility/ImPath.h \
    Utility/LogBuffer.h \
    Utility/LogMacro.h \
    Utility/LogUtil.h \
    datamodel.h \
    downloadimagecontroller.h \
    housedialog.h \
    househttpclient.h \
    httpclientbase.h \
    imagedialog.h \
    loginwindow.h \
    mainwindow.h \
    modifypassworddialog.h \
    myprogressdialog.h \
    pathutil.h \
    settingmanager.h \
    uiutil.h \
    uploadimagecontroller.h

FORMS += \
    housedialog.ui \
    imagedialog.ui \
    loginwindow.ui \
    mainwindow.ui \
    modifypassworddialog.ui

# Enable PDB generation
QMAKE_CFLAGS_RELEASE += /Zi
QMAKE_CXXFLAGS_RELEASE += /Zi
QMAKE_LFLAGS_RELEASE += /DEBUG

# Enable log context
DEFINES += QT_MESSAGELOGCONTEXT

RESOURCES += \
    resource.qrc

RC_ICONS = res\logo.ico
