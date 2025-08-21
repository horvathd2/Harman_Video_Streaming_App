A simple cross-platform video streaming application in C using FFMPEG libraries, which outputs a .h264 file aswell that can be played.

Required libraries:

1) FFMPEG - webcam video capturing an processing

Ubuntu installation:

sudo apt update
sudo apt install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libavdevice-dev

Windows installation:



2) SDL2 - video stream displaying on a window

Ubuntu installation:

sudo apt install libsdl2-dev

Windows installation:




Use ffplay output.h264 to play the h264 file on Ubuntu.
