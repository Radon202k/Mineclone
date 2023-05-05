@echo off

IF NOT EXIST bin mkdir bin

set filename=../platform/windows_opengl/windows_opengl.c

pushd bin
cl -Od -Z7 -FC -MTd -W4 -wd4100 -wd4152 -wd4201 -nologo %filename%
REM cl -O2 -FC -MT -W4 -wd4100 -wd4152 -wd4201 -nologo %filename%
popd