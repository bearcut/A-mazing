@echo off
if not exist build mkdir build
if not exist bin mkdir bin

echo Compiling...
gcc -Wall -Wextra -Iinclude src\*.c -o bin\a_mazing.exe

echo Done!
