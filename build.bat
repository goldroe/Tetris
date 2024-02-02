@echo off

IF NOT EXIST build MKDIR build
SET includes=/Iext\raylib\include
SET warning_flags=/W4 /WX /wd4100 /wd4189
SET compiler_flags=/nologo /FC /Zi /MDd %warning_flags% %includes% /FeTetris.exe /Fobuild\
SET source_files=src\tetris.cpp

SET libs=raylib.lib user32.lib shell32.lib gdi32.lib winmm.lib user32.lib
SET linker_flags=/INCREMENTAL:NO /LIBPATH:ext\raylib\lib\ /NODEFAULTLIB:msvcrt.lib %libs%

CL %compiler_flags% %source_files% /link %linker_flags%
