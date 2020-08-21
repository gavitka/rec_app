SET(CMAKE_FIND_LIBRARY_SUFFIXES "" ".lib" ".a" ".dll")

SET(LIBX264_BUILD
    C:/dev/lib/x264_build
    D:/dev/lib/x264_build
    )

find_path(
    LIBX264_INCLUDE_DIR
        x264.h
    PATHS
        ${LIBX264_BUILD}
    PATH_SUFFIXES
        include/
    REQUIRED
    )

find_library(
    FFMPEG_LIBX264
    NAMES
        libx264
    PATHS
        ${LIBX264_BUILD}
    PATH_SUFFIXES
        lib/
    REQUIRED
    )
