A simple cross-platform video streaming application in C using FFMPEG libraries, which outputs a .h264 file aswell that can be played.

# INSTALLATION

Required libraries: ffmpeg and SDL2 (MinGW version for Windows)

## 1) FFMPEG - webcam video capturing an processing

### Ubuntu installation:

> *sudo apt update\
sudo apt install ffmpeg libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libavdevice-dev*

(optional)
> *sudo apt install pkg-config*

### Windows installation:

Download the precompiled FFMPEG builds\
Extract the zip\
Copy the include folder from the FFMPEG folder into your project\
Copy the .lib files from the lib forlder\
Copy the bin/ffmpeg.dll somewhere in your PATH alongside the executable

Or use:
> *choco install ffmpeg*


## 2) SDL2 - video stream displaying on a window

### Ubuntu installation:

> *sudo apt install libsdl2-dev*

### Windows installation:

Download the SDL2 development libraries (MinGW version)\
Extract the zip\
Add the library folder to PATH\
Place SDL2.dll alongside your executable


# BUILDING

## Ubuntu:

Navigate to the build folder and run the following commands in a terminal:

> *cmake ..\
make
./harman_video_app*

## Windows:

Navigate to the src folder in a terminal and run the following command:

> *gcc main.c -IC:\ffmpeg\ffmpeg-master-latest-win64-gpl-shared\include 
                      -IC:\SDL2\x86_64-w64-mingw32\include 
                      -LC:\ffmpeg\ffmpeg-master-latest-win64-gpl-shared\lib 
                      -LC:\SDL2\x86_64-w64-mingw32\lib 
                      -lavformat -lavcodec -lavutil -lswscale -lavdevice -lSDL2 -lws2_32 -ldxguid -ldinput8 -ldxerr8 -o main.exe*


Use *ffplay output.h264* to play the h264 file on Ubuntu.
