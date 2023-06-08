all: main

main:
# RELEASE
	windres shrunk.rc -o build/shrunk.rc.data --target=pe-x86-64
	gcc src/*.c build/shrunk.rc.data -o build/Shrunk.exe -O1 -Wall -Wextra -std=c11 -Wno-missing-braces -Werror=implicit-function-declaration -mwindows -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
# DEBUG
#	gcc src/*.c tests/*.c -o build/Shrunk.exe -g -O0 -std=c11 -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm

run: main
	./build/Shrunk.exe

.PHONY: all run