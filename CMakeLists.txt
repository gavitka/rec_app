cmake_minimum_required(VERSION 3.18)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(ffmpeg REQUIRED)
find_package(x264 REQUIRED)

include_directories(
    ${FFMPEG_INC_DIR}
    ${LIBX264_INCLUDE_DIR}
)

project("time_lapse_rec")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

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
    src/capturethread.cpp
    src/capturethread.h
    src/kheventfilter.h
    src/performancetimer.cpp
    src/performancetimer.h
    src/applist.cpp
    src/applist.h
    src/pch.h
    src/lib.h
    src/lib.cpp
    src/blwindow.h
    src/blwindow.cpp

    lib/wintoastlib.h
    lib/wintoastlib.cpp

    src/hooks.h
    src/hooks.cpp

    README.md
    installer/installer.nsi

    ${SRC_UI}
    ${RESOURCES}
    ${QTRESOURCES}
    )

target_precompile_headers(${PROJECT_NAME}
    PUBLIC src/pch.h
    )

source_group("ui" FILES ${SRC_UI})

set(PROJECT_DLL "hooks_dll")

set(QML_IMPORT_PATH 
    "${CMAKE_SOURCE_DIR}/src/imports" 
    CACHE STRING "" FORCE)

add_library(${PROJECT_DLL} SHARED
    src/hooks_dll/dllmain.cpp
    src/hooks_dll/mousehook.cpp
    src/hooks_dll/mousehook.h
    )

target_compile_definitions(${PROJECT_DLL} PUBLIC MOUSEHOOK_EXPORTS)

set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")

# Do i really need to link all this SHIT?

set(LIBS
    Qt5::Widgets
    Qt5::Core
    Qt5::Qml
    Qt5::Quick
    Qt5::Multimedia
    Qt5::WinExtras
    dwmapi
    ${FFMPEG_LIBX264}
    ${FFMPEG_LIBAVCODEC}
    ${FFMPEG_LIBAVDEVICE}
    ${FFMPEG_LIBAVFILTER}
    ${FFMPEG_LIBAVFORMAT}
    ${FFMPEG_LIBAVUTIL}
    ${FFMPEG_LIBPOSTPROC}
    ${FFMPEG_LIBSWRESAMPLE}
    ${FFMPEG_LIBSWSCALE}
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
    Mfcore
    Mfplat
    mfplay
    mfreadwrite
    mfsensorgroup
    Mfsrcsnk
    mfuuid
    Kernel32
    )

target_link_libraries(${PROJECT_NAME} ${LIBS})

target_link_libraries(${PROJECT_NAME} ${PROJECT_DLL})

target_link_libraries(${PROJECT_DLL} Qt5::Core)

# set startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

if(CMAKE_BUILD_TYPE STREQUAL "Release")

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
