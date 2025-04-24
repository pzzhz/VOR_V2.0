@echo off
setlocal enabledelayedexpansion


set "file=version.txt"
set "header_file=..\INTERFACE\UI\control\version.h"

if not exist "%file%" (
    echo 1 > "%file%"
    exit /b
)

if not exist "%header_file%" (
    echo 1 > "%header_file%"
    exit /b
)

:: 读取文件中的数字
set /p number=<"%file%"

:: 确保是数字
for /f "delims=" %%a in ('echo %number%') do set /a number=%%a+1

:: 写入新的数字
echo %number% > "%file%"

:: 输出新的数字
echo current version : %number%

(
    echo #ifndef NUMBER_H
    echo #define NUMBER_H
    echo.
    echo #define Version_NUMBER %number%
    echo.
    echo #endif
) > "%header_file%"
