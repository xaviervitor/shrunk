all: main

main:
	windres shrunk.rc -o build/shrunk.rc.data --target=pe-x86-64
	gcc src/*.c build/shrunk.rc.data -o build/Shrunk.exe -O1 -Wall -Wextra -std=c11 -Wno-missing-braces -Werror=implicit-function-declaration -mwindows -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm

run: main
	./build/Shrunk.exe

.PHONY: all run