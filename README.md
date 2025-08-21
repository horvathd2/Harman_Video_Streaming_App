A simple cross-platform video streaming application in C using FFMPEG libraries, which outputs a .h264 file aswell that can be played.

INSTALLATION

Required libraries:

1) FFMPEG - webcam video capturing an processing

Ubuntu installation:

sudo apt update
sudo apt install ffmpeg libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libavdevice-dev

(optional)
sudo apt install pkg-config

Windows installation:

Download the precompiled FFMPEG builds
Extract the zip
Copy the include folder from the FFMPEG folder into your project
Copy the .lib files from the lib forlder
Copy the bin/ffmpeg.dll somewhere in your PATH alongside the executable

Or use: choco install ffmpeg


2) SDL2 - video stream displaying on a window

Ubuntu installation:

sudo apt install libsdl2-dev

Windows installation:

Download the SDL2 development libraries
Extract the zip
Copy include/SDL2 to your compiler include path
Copy .lib (or .a for MinGW) to your library path
Place SDL2.dll alongside your executable


COMPILATION

Ubuntu:

Navigate to the build folder in the project and run the following commands in a terminal:

cmake ..
make
./harman_video_app

Windows:

gcc main.c -o video_app \
    -I"C:/ffmpeg/include" -I"C:/SDL2/include" \
    -L"C:/ffmpeg/lib" -L"C:/SDL2/lib" \
    -lavformat -lavcodec -lavutil -lswscale -lavdevice -lSDL2 -lws2_32 -ldxguid -ldinput8 -ldxerr8



Use ffplay output.h264 to play the h264 file on Ubuntu.
