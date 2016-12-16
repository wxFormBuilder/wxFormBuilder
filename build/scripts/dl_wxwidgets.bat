rem Usage:
rem   dl_wxwidgets.bat <version> <compiler> <cpu>
rem For example:
rem   dl_wxwidgets.bat 3.0.2 vc120 x86

mkdir wxwidgets
pushd wxwidgets

set VERSION=%1
set COMPILER=%2
set CPU=%3

set BASE_URL=https://github.com/wxWidgets/wxWidgets/releases/download/v%VERSION%

set HEADER_SUFFIX=-headers
if "%VERSION%"=="3.0.2" set HEADER_SUFFIX=_headers

curl -L -o hdr.7z %BASE_URL%/wxWidgets-%VERSION%%HEADER_SUFFIX%.7z
7z x hdr.7z
del hdr.7z

curl -L -o dev.7z %BASE_URL%/wxMSW-%VERSION%_%COMPILER%_Dev.7z
7z x dev.7z
del dev.7z
rem FindwxWidgets.cmake does not handle compiler specifc vc_ folders
move lib\%COMPILER%_dll lib\vc_dll

popd wxwidgets
