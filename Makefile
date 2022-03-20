CFLAGS = -Wall -std=c++14 -O3
C = g++

FILES = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,%.o,$(FILES))
HEADERS = $(wildcard *.h)

SDLFLAGS = $(shell sdl2-config --libs --cflags) -lSDL2_image -lm

emu: $(HEADERS) $(FILES) $(OBJS)
	$(C) -o emu.exe $(OBJS) $(CFLAGS) $(SDLFLAGS)

$(OBJS): %.o: %.cpp $(HEADERS)
	$(C) -c $(patsubst %.o,%.cpp,$@) -o $@ $(CFLAGS)