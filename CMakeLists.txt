cmake_minimum_required(VERSION 3.17)

set(CMAKE_C_COMPILER C:/msys64/mingw64/bin/gcc.exe)
set(CMAKE_CXX_COMPILER C:/msys64/mingw64/bin/g++.exe)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project("time_lapse_rec")

find_package(ffmpeg REQUIRED)
find_package(x264 REQUIRED)

include_directories(
    ${FFMPEG_INC_DIR}
    ${LIBX264_INCLUDE_DIR}
)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DMESSAGE_HANDLER) # debug to c:/temp/log.txt

find_package(Qt5 COMPONENTS
    Widgets
    Quick
    Core
    QuickControls2
    Multimedia
    winextras
    REQUIRED)

qt5_add_resources(QTRESOURCES src/res.qrc)

file(GLOB_RECURSE SRC_UI
    src/main.qml
    src/notification.qml
    src/qtquickcontrols2.conf
    src/imports/Theme/Theme.qml
    src/Components/AppList.qml
    src/Components/AppButton.qml
    src/Components/OpenFolderDialog.qml
    src/Components/PauseButton.qml
    src/Components/RecButton.qml
    src/Components/TopButton.qml
    src/Components/RefreshButton.qml
    src/Components/NotificationWindow.qml
)

set(RESOURCES
    src/res.rc
)

add_executable(${PROJECT_NAME} WIN32 
    src/main.cpp
    src/applistmodel.cpp
    src/applistmodel.h
    src/backend.cpp
    src/backend.h
    src/captureworker.cpp
    src/captureworker.h
    src/kheventfilter.h
    src/performancetimer.cpp
    src/performancetimer.h
    src/appmanager.cpp
    src/appmanager.h
    src/pch.cpp
    src/pch.h
    src/lib.cpp
    src/lib.h
    src/blwindow.cpp
    src/blwindow.h

    #src/lib/wintoastlib.cpp
    #src/lib/wintoastlib.h

    README.md
    installer/installer.nsi

    ${SRC_UI}
    ${RESOURCES}
    ${QTRESOURCES}
    )

source_group("ui" FILES ${SRC_UI})

set(QML_IMPORT_PATH 
    "${CMAKE_SOURCE_DIR}/src/imports" 
    CACHE STRING "" FORCE)

set(PROJECT_DLL "hooks_dll")

add_library(${PROJECT_DLL} SHARED
    src/hooks_dll/hook.cpp
    src/hooks_dll/hook.h
    )

target_include_directories(${PROJECT_DLL} PUBLIC
    "${CMAKE_SOURCE_DIR}/src/"
    )

#target_include_directories(${PROJECT_NAME} PUBLIC
#    "C:/Program Files (x86)/Windows Kits/10/Include/10.0.18362.0/winrt"
#    )

target_precompile_headers(${PROJECT_NAME}
    PUBLIC src/pch.h
    )

target_precompile_headers(${PROJECT_DLL}
    PUBLIC src/pch.h
    )

target_compile_definitions(${PROJECT_DLL} PUBLIC DLLEXPORT)

# removed for minGW
# set(CMAKE_EXE_LINKER_FLAGS
#    "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")

# Do i really need to link all this SHIT?

set(LIBS
    Qt5::Widgets
    Qt5::Core
    Qt5::Qml
    Qt5::Quick
    Qt5::Multimedia
    Qt5::WinExtras
    dwmapi
    ${FFMPEG_LIBAVFORMAT}
    ${FFMPEG_LIBAVCODEC}
    ${FFMPEG_LIBSWRESAMPLE}
    ${FFMPEG_LIBSWSCALE}
    ${FFMPEG_LIBAVUTIL}
    ${FFMPEG_LIBAVFILTER}
    ${FFMPEG_LIBAVDEVICE}
    ${FFMPEG_LIBPOSTPROC}
    bcrypt
    gdi32
    kernel32
    user32
    winspool
    comdlg32
    advapi32
    shell32
    ole32
    oleaut32
    uuid
    odbc32
    odbccp32
    Ws2_32
    Secur32
    Strmiids
    Mf
#    Mfcore
    Mfplat
#    mfplay
    mfreadwrite
#    mfsensorgroup
#    Mfsrcsnk
    mfuuid
    Kernel32
    ao
    m
    z
    lzma
    bz2
#    va
    #rt
    iconv
    ${FFMPEG_LIBX264}
    )

target_link_libraries(${PROJECT_NAME} ${LIBS})

target_link_libraries(${PROJECT_NAME} ${PROJECT_DLL})

target_link_libraries(${PROJECT_DLL} Qt5::Core)

# set startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# if(CMAKE_BUILD_TYPE STREQUAL "Release")

if(TRUE)
# RUNNUNG WINDEPLOYQT

# Retrieve the absolute path to qmake and then use that path to find
# the binaries

     get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
     get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
     find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

     add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
         COMMAND "${CMAKE_COMMAND}" -E
             env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
                 "$<TARGET_FILE:${PROJECT_NAME}>" ARGS --qmldir ${CMAKE_SOURCE_DIR}/src/
         COMMENT "Running windeployqt..."
     )

endif()
