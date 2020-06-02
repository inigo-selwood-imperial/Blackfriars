#!bin/bash

g++ ..\\source\\main.cpp ..\\source\\component.cpp -std=c++11 -Wall -g3 -o blackfriars.exe

if [ $? -eq 0 ]
then
    .\\blackfriars.exe
fi
