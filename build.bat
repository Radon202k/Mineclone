@echo off

IF NOT EXIST bin mkdir bin

pushd bin
cl -Od -Z7 -FC -MTd -W4 -wd4100 -wd4152 -wd4201 -nologo ../platform/windows_opengl/windows_opengl.c
popd