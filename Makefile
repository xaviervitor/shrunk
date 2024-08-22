project_name := shrunk
output := $(project_name).exe

all: $(output)

# compile variables
cc := gcc
std := -std=c17
warnings := -Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wdisabled-optimization -Winit-self \
-Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls -Wshadow \
-Wsign-conversion -Wswitch-default -Wundef -Wno-unused

# build flags
build_flags.debug := -O0 -ggdb3
build_flags.release := -O3 -mwindows
extra_flags := ${build_flags.${build}}

# get all src/*.c
sources := $(wildcard src/*.c)
# create a list of build/%.o based on sources list
objects := $(patsubst src/%.c, build/%.o, $(sources))
# create a list of build/%.d based on sources list
depends := $(patsubst src/%.c, build/%.d, $(sources))

# link all compiled objects
$(output): $(objects) build/$(project_name).rc.data
	@$(cc) $^ -o $@ $(std) $(warnings) $(extra_flags) \
	-L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
	@echo $@

-include $(depends)

# build all src/%.c to build/%.o
build/%.o: src/%.c Makefile
	@$(cc) -c $< -o $@ $(std) $(warnings) $(extra_flags) -MMD -MP -I src/include/ -isystem include/
	@echo "$< > $@"

# make project icon
build/$(project_name).rc.data: resources/icon/$(project_name).rc
	@windres $^ -o $@ --target=pe-x86-64
	@echo "windres $^ > $@"

run: $(output)
	./$(output)

clean:
	@rm -f $(output)
	@echo "rm output"
	@rm -f $(objects)
	@echo "rm objects"
	@rm -f $(depends)
	@echo "rm depends"

.PHONY: all run clean