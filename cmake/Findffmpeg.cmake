SET(CMAKE_FIND_LIBRARY_SUFFIXES "" ".lib" ".a" ".dll")

SET(FFMPEG_BUILD
C:/dev/lib/ffmpeg_build/
D:/dev/lib/ffmpeg_build/
)

# For some fucking piece-of-shit stupid reason
# i can't name this variable FFMPEG_INCLUDE_DIR

find_path(
FFMPEG_INC_DIR
    libavcodec/avcodec.h
PATHS
    ${FFMPEG_BUILD}
PATH_SUFFIXES
    inc/
REQUIRED
)

find_library(
FFMPEG_LIBAVCODEC
NAMES
    libavcodec
PATHS
    ${FFMPEG_BUILD}
PATH_SUFFIXES
    prefix/lib/
REQUIRED
)

find_library(
FFMPEG_LIBAVDEVICE
NAMES
    libavdevice
PATHS
    ${FFMPEG_BUILD}
PATH_SUFFIXES
    prefix/lib/
REQUIRED
)


find_library(
FFMPEG_LIBAVFILTER
NAMES
    libavfilter
PATHS
    ${FFMPEG_BUILD}
PATH_SUFFIXES
    prefix/lib/
REQUIRED
)


find_library(
FFMPEG_LIBAVFORMAT
NAMES
    libavformat
PATHS
    ${FFMPEG_BUILD}
PATH_SUFFIXES
    prefix/lib/
REQUIRED
)

find_library(
FFMPEG_LIBAVUTIL
NAMES
    libavutil
PATHS
    ${FFMPEG_BUILD}
PATH_SUFFIXES
    prefix/lib/
REQUIRED
)

find_library(
FFMPEG_LIBPOSTPROC
NAMES
    libpostproc
PATHS
    ${FFMPEG_BUILD}
PATH_SUFFIXES
    prefix/lib/
REQUIRED
)

find_library(
FFMPEG_LIBSWRESAMPLE
NAMES
    libswresample
PATHS
    ${FFMPEG_BUILD}
PATH_SUFFIXES
    prefix/lib/
REQUIRED
)

find_library(
FFMPEG_LIBSWSCALE
NAMES
    libswscale
PATHS
    ${FFMPEG_BUILD}
PATH_SUFFIXES
    prefix/lib/
REQUIRED
)