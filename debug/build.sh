#!bin/bash

# g++ ..\\source\\main.cpp -std=c++11 -Wall -g3 -O2  -lmingw32 -lSDL2main \
#       -lSDL2 -lSDL2_image -lSDL2_ttf -o blackfriars.exe

g++ ..\\source\\main.cpp -std=c++11 -Wall -g3 -o blackfriars.exe

if [ $? -eq 0 ]
then
    .\\blackfriars.exe
fi
