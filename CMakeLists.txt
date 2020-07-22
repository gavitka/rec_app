cmake_minimum_required(VERSION 3.18)

include_directories(
    "D:/dev/lib/ffmpeg_build/inc"
    "D:/dev/lib/x264/"
    )

link_directories(
    "D:/dev/lib/ffmpeg_build/prefix/lib"
    "D:/dev/lib/x264/installed/lib/"
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
    src/qtquickcontrols2.conf
    src/imports/Theme/Theme.qml
    src/Components/AppList.qml
    src/Components/OpenFolderDialog.qml
    src/Components/PauseButton.qml
    src/Components/RecButton.qml
    src/Components/TopButton.qml
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
    src/VideoCapture.cpp
    src/VideoCapture.h
    src/perfomancetimer.cpp
    src/perfomancetimer.h

    README.md
    installer/installer.nsi

    ${SRC_UI}
    ${RESOURCES}
    ${QTRESOURCES}
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

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::Core)
target_link_libraries(${PROJECT_NAME} Qt5::Qml)
target_link_libraries(${PROJECT_NAME} Qt5::Quick)
target_link_libraries(${PROJECT_NAME} Qt5::Multimedia)
target_link_libraries(${PROJECT_NAME} Qt5::WinExtras)

target_link_libraries(${PROJECT_NAME} D:/dev/lib/x264/installed/lib/libx264.lib)
target_link_libraries(${PROJECT_NAME} D:/dev/lib/ffmpeg_build/prefix/lib/libavcodec.a)
target_link_libraries(${PROJECT_NAME} D:/dev/lib/ffmpeg_build/prefix/lib/libavdevice.a)
target_link_libraries(${PROJECT_NAME} D:/dev/lib/ffmpeg_build/prefix/lib/libavfilter.a)
target_link_libraries(${PROJECT_NAME} D:/dev/lib/ffmpeg_build/prefix/lib/libavformat.a)
target_link_libraries(${PROJECT_NAME} D:/dev/lib/ffmpeg_build/prefix/lib/libavutil.a)
target_link_libraries(${PROJECT_NAME} D:/dev/lib/ffmpeg_build/prefix/lib/libpostproc.a)
target_link_libraries(${PROJECT_NAME} D:/dev/lib/ffmpeg_build/prefix/lib/libswresample.a)
target_link_libraries(${PROJECT_NAME} D:/dev/lib/ffmpeg_build/prefix/lib/libswscale.a)
target_link_libraries(${PROJECT_NAME} bcrypt)
target_link_libraries(${PROJECT_NAME} gdi32)
target_link_libraries(${PROJECT_NAME} kernel32)
target_link_libraries(${PROJECT_NAME} user32)
target_link_libraries(${PROJECT_NAME} winspool)
target_link_libraries(${PROJECT_NAME} comdlg32)
target_link_libraries(${PROJECT_NAME} advapi32)
target_link_libraries(${PROJECT_NAME} shell32)
target_link_libraries(${PROJECT_NAME} ole32)
target_link_libraries(${PROJECT_NAME} oleaut32)
target_link_libraries(${PROJECT_NAME} uuid)
target_link_libraries(${PROJECT_NAME} odbc32)
target_link_libraries(${PROJECT_NAME} odbccp32)
target_link_libraries(${PROJECT_NAME} Ws2_32)
target_link_libraries(${PROJECT_NAME} Secur32)
target_link_libraries(${PROJECT_NAME} Strmiids)

# Do i really need to link all this SHIT?

target_link_libraries(${PROJECT_NAME} Mf)
target_link_libraries(${PROJECT_NAME} Mfcore)
target_link_libraries(${PROJECT_NAME} Mfplat)
target_link_libraries(${PROJECT_NAME} mfplay)
target_link_libraries(${PROJECT_NAME} mfreadwrite)
target_link_libraries(${PROJECT_NAME} mfsensorgroup)
target_link_libraries(${PROJECT_NAME} Mfsrcsnk)
target_link_libraries(${PROJECT_NAME} mfuuid)

target_link_libraries(${PROJECT_NAME} ${PROJECT_DLL})

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
