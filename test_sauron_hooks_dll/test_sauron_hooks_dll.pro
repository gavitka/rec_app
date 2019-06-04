TEMPLATE = lib

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    dllmain.cpp \
    mousehook.cpp

HEADERS += \
    mousehook.h

INCLUDEPATH += C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC\14.21.27702\include
INCLUDEPATH += C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Tools\MSVC\14.21.27702\atlmfc\include
INCLUDEPATH += C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\VS\include
INCLUDEPATH += C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\ucrt
INCLUDEPATH += C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\um
INCLUDEPATH += C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\shared
INCLUDEPATH += C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\winrt
INCLUDEPATH += C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\cppwinrt
INCLUDEPATH += C:\Program Files (x86)\Windows Kits\NETFXSDK\4.7.2\Include\um

LIBS += kernel32.lib
LIBS += user32.lib
LIBS += gdi32.lib
LIBS += winspool.lib
LIBS += comdlg32.lib
LIBS += advapi32.lib
LIBS += shell32.lib
LIBS += ole32.lib
LIBS += oleaut32.lib
LIBS += uuid.lib
LIBS += odbc32.lib
LIBS += odbccp32.lib
LIBS += WindowsApp.lib

DEFINES += MOUSEHOOK_EXPORTS
