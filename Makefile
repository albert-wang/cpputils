

CC := ccache clang++
LD := clang++
CFLAGS = -I./include -I/opt/local/include -Wall -std=c++0x -fcolor-diagnostics -g
OUTPUT = alloc

SRC_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(patsubst src/%.cpp, obj/%.o, $(SRC_FILES))

all : main

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) -o $@ -c $< 

main : $(OBJ_FILES)
	mkdir -p obj
	$(LD) -o $(OUTPUT) $(OBJ_FILES)

clean : 
	rm $(OUTPUT); rm obj/*
