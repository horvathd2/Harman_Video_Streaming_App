cmake_minimum_required(VERSION 3.10)
project(harman_video_app C)

set(CMAKE_C_STANDARD 11)

if(WIN32)
    #FFMPEG
    set(FFMPEG_DIR "C:/ffmpeg")
    include_directories(${FFMPEG_DIR}/include)
    link_directories(${FFMPEG_DIR}/lib)

    #SDL2
    set(SDL2_DIR "C:/SDL2")
    include_directories(${SDL2_DIR}/include)
    link_directories(${SDL2_DIR}/lib)

else()
    #FFMPEG
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(FFMPEG REQUIRED
        libavcodec
        libavformat
        libavutil
        libswscale
        libavdevice
    )

    #SDL2
    pkg_check_modules(SDL2 REQUIRED sdl2)

    include_directories(
        ${FFMPEG_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
    )

    link_directories(
        ${FFMPEG_LIBRARY_DIRS}
        ${SDL2_LIBRARY_DIRS}  
    )

endif()

add_executable(harman_video_app src/main.c)

if(WIN32)
    target_link_libraries(harman_video_app
        avformat
        avcodec
        avutil
        swscale
        avdevice
        SDL2
    )
else()
    target_link_libraries(harman_video_app
        ${FFMPEG_LIBRARIES}
        ${SDL2_LIBRARIES}  
    )
endif()