all: main

main:
	gcc src/*.c -o build/game.exe -O1 -Wall -Wextra -std=c11 -Wno-missing-braces -Werror=implicit-function-declaration -mwindows -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm

run: main
	./build/game.exe

.PHONY: all run