all: main

main:
# RELEASE
	windres shrunk.rc -o build/shrunk.rc.data --target=pe-x86-64
	gcc -o build/Shrunk.exe src/*.c build/shrunk.rc.data -Wall -Wextra -O2 -std=c17 -Wno-missing-braces -Werror=implicit-function-declaration -mwindows -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
# DEBUG
#	gcc -o build/Shrunk.exe src/*.c tests/*.c -g -Og -Wall -Wextra -O2 -std=c17 -Wno-missing-braces -Werror=implicit-function-declaration -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm

run: main
	./build/Shrunk.exe

.PHONY: all run