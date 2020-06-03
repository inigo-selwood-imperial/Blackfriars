#!bin/bash

g++ ..\\source\\main.cpp -o blackfriars.exe ..\\source\\complex.cpp ..\\source\\component.cpp -std=c++11 -Wall -g3

if [ $? -eq 0 ]
then
    .\\blackfriars.exe
fi
