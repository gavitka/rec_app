QT += quick
QT += widgets
QT += winextras
QT += gui
QT += quickcontrols2
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        VideoCapture.cpp \
        applistmodel.cpp \
        backend.cpp \
        capturethread.cpp \
        main.cpp \
        perfomancetimer.cpp

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

QML_IMPORT_PATH = $$PWD/imports
QML_IMPORT_PATH += $$PWD/Components

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    VideoCapture.h \
    applistmodel.h \
    backend.h \
    capturethread.h \
    kheventfilter.h \
    pch.h \
    perfomancetimer.h

LIBS += "C:\dev\lib\ffmpeg_prefix\lib\libavcodec.a"
LIBS += "C:\dev\lib\ffmpeg_prefix\lib\libavutil.a"
LIBS += "C:\dev\lib\ffmpeg_prefix\lib\libavformat.a"
LIBS += "C:\dev\lib\ffmpeg_prefix\lib\libswresample.a"
LIBS += "C:\dev\lib\ffmpeg_prefix\lib\libswscale.a"
LIBS += "C:\dev\lib\ffmpeg_prefix\lib\libavdevice.a"
LIBS += "C:\dev\lib\ffmpeg_prefix\lib\libavfilter.a"
LIBS += "C:\dev\lib\ffmpeg_prefix\lib\libpostproc.a"
LIBS += "C:\dev\lib\x264\installed\lib\libx264.lib"
LIBS += bcrypt.lib
LIBS += gdi32.lib
LIBS += kernel32.lib
LIBS += user32.lib
LIBS += winspool.lib
LIBS += comdlg32.lib
LIBS += advapi32.lib
LIBS += shell32.lib
LIBS += ole32.lib
LIBS += oleaut32.lib
LIBS += uuid.lib
LIBS += odbc32.lib
LIBS += odbccp32.lib
LIBS += Ws2_32.lib
LIBS += Secur32.lib

INCLUDEPATH += C:\dev\lib\ffmpeg_inc\
INCLUDEPATH += C:\dev\lib\x264

#mousehook dll
INCLUDEPATH += ..\hooks_dll
LIBS += -L..\build_dll\release -lhooks_dll

#PRECOMPILED_HEADER = pch.h

QMAKE_LFLAGS += /NODEFAULTLIB:library

DISTFILES =

msvc:release {
    QMAKE_CXXFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /DEBUG
}

RC_ICONS = images\logo.ico

RESOURCES += \
    qml.qrc
