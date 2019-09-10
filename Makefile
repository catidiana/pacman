PACKAGES = sdl2 SDL2_image SDL2_mixer
CFLAGS = -Wall -Wno-unused-function -std=c++11
LDLIBS = -lm
MACROS =

ifeq ($(OS), Windows_NT)
   MACROS += -DOS_WINDOWS
   LDLIBS += -lopengl32 -mwindows
else
   MACROS += -DOS_GNULINUX
   LDLIBS += -pthread
   PACKAGES += gl
endif

CFLAGS += $(shell pkg-config --cflags $(PACKAGES))
LDLIBS += $(shell pkg-config --libs   $(PACKAGES))

pacman: src/main.cpp src/*.cpp
	g++ $(MACROS) $(CFLAGS) -o $@ $< $(LDLIBS) 
