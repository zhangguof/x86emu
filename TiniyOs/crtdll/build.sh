#!/bin/sh
cd /Volumes/Transcend/vs2019/vs2019
export INCLUDE="$PWD/VC/Tools/MSVC/14.22.27905/include;
#$PWD/win8sdk/Include/shared/;$PWD/win8sdk/Include/um/;$PWD/win8sdk/Include/winrt/"
export LIB="$PWD/VC/Tools/MSVC/14.22.27905/lib/x86;
# $PWD/win8sdk/Lib/winv6.3/um/x86"
# echo "#include <iostream>" > test1.cpp
# echo "int main() { std::cout << \"Hello from MSVC\"; }" >> test1.cpp
export WINEDEBUG=-all # to turn off wine's warnings about unimplemented stuff
# cp sys32/msvcp140.dll VC/bin # need the native version of this DLL
export WINEDLLOVERRIDES="*msvcp140=n" # tell wine to use the native DLL
wine VC/Tools/MSVC/14.22.27905/bin/Hostx86/x86/cl.exe test1.cpp
