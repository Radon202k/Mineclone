#!/bin/bash

pushd bin
gcc -g -I ~/Work/libs ~/Desktop/Mineclone/platform/linux_opengl/linux_opengl.c -o x11_opengl -lm -lX11 -lEGL 
popd