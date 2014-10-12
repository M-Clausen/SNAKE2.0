CC 		= g++
CFLAGS 	= -c -I./include -std=c++11

LINKER 	= gcc
BIN 	= snake
LFLAGS 	= -o $(BIN) -lstdc++ -lSDL2 -lGL

HEADERS = \
./include/keyboard.h \
./include/draw.h \
./include/polygon.h \
./include/game.h \
./include/map.h \
./include/snake.h \
./include/timer.h \
./include/events.h \
./include/math/vec2.hpp \
./include/math/vec3.hpp \
./include/math/vec4.hpp \
./include/math/mat2.hpp \
./include/math/mat3.hpp \
./include/math/mat4.hpp

OBJS 	= \
./obj/main.o \
./obj/draw.o \
./obj/game.o \
./obj/map.o \
./obj/keyboard.o \
./obj/snake.o \
./obj/events.o \
./obj/timer.o

default: prepare $(OBJS)
	$(LINKER) $(OBJS) $(LFLAGS)

prepare:
	mkdir -p obj/

run: default
	./snake

obj/main.o: src/main.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

obj/draw.o: src/video/draw.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

obj/game.o: src/game/game.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

obj/map.o: src/game/map.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

obj/snake.o: src/game/snake.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

obj/keyboard.o: src/io/keyboard.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

obj/timer.o: src/timer.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

obj/events.o: src/game/events.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@