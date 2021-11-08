call C:\Qt\6.2.1\msvc2019_64\bin\qtenv2.bat
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
cd %~dp0
rmdir /s /q vs-sol\
qmake -Wall -tp vc -o vs-sol\smarter_gui_demo.vcxproj smarter_gui_demo.pro
devenv "vs-sol\smarter_gui_demo.vcxproj"
