CC 		= g++
CFLAGS 	= -c -I./include -std=c++11

LINKER 	= gcc
BIN 	= snake
LFLAGS 	= -o $(BIN) -lstdc++ -lSDL2 -lGL -lGLEW -lm

HEADERS = \
./include/keyboard.h \
./include/render.h \
./include/polygon.h \
./include/game.h \
./include/map.h \
./include/snake.h \
./include/shader.h \
./include/timer.h \
./include/events.h \
./include/math/math.hpp \
./include/math/vec2.hpp \
./include/math/vec3.hpp \
./include/math/vec4.hpp \
./include/math/mat2.hpp \
./include/math/mat3.hpp \
./include/math/mat4.hpp

OBJS 	= \
./obj/main.o \
./obj/render.o \
./obj/game.o \
./obj/shader.o \
./obj/map.o \
./obj/keyboard.o \
./obj/snake.o \
./obj/events.o \
./obj/timer.o \
./obj/math.o \
./obj/mat3.o \
./obj/mat4.o

default: prepare $(OBJS)
	$(LINKER) $(OBJS) $(LFLAGS)

prepare:
	mkdir -p obj/

run: default
	./snake

obj/main.o: src/main.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

obj/render.o: src/graphics/render.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

obj/shader.o: src/graphics/shader.cpp $(HEADERS)
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

obj/math.o: src/math/math.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

obj/mat3.o: src/math/mat3.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

obj/mat4.o: src/math/mat4.cpp $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@