cmake_minimum_required(VERSION 3.10)
project(harman_video_app C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)

pkg_check_modules(FFMPEG REQUIRED
    libavcodec
    libavformat
    libavutil
    libswscale
    libavdevice
)

pkg_check_modules(SDL2 REQUIRED sdl2)

include_directories(
    ${FFMPEG_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS}
)

link_directories(
    ${FFMPEG_LIBRARY_DIRS}
    ${SDL2_LIBRARY_DIRS}  
)

add_executable(harman_video_app src/main.c)

target_link_libraries(harman_video_app
    ${FFMPEG_LIBRARIES}
    ${SDL2_LIBRARIES}  
)