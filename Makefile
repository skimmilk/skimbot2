CXX=g++
CC=gcc
# Get time in nanoseconds, good enough randomness
RANDOM=$(shell date +%N)
CFLAGS=-c -Wall -Wextra -g -fpic -m32 -O0 -std=c++11 -I src/ -Wfatal-errors -DPOSIX -DRANDOM=$(RANDOM)
LDFLAGS=-m32 -shared -ldl -lm
SOURCES=src/main.cpp\
	src/sourceutil.cpp\
	src/sigscan.cpp\
	src/ifs.cpp\
	src/cvarhook.cpp\
	src/debug.cpp\
	src/sdk/cvar.cpp\
	src/netvar.cpp\
	src/vthook.cpp\
	src/sdk/crc.cpp\
	src/basehook.cpp\
	src/tfdebug.cpp\
	src/exit.cpp\
	src/trace.cpp\
	src/trigger.cpp\
	src/bhop.cpp\
	src/trig.cpp\
	src/spin.cpp\
	src/sdk/math.cpp\
	src/skeltal.cpp\
	src/draw.cpp\
	src/tfplayer.cpp\
	src/esp.cpp\
	src/steam.cpp\
	src/achievement.cpp\
	src/aim.cpp\
	src/nospread.cpp\
	src/tfcache.cpp
OBJECTS=$(SOURCES:.cpp=.o)
HACK=libskimbot.so

all: $(HACK)

$(HACK): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) 

.cpp.o:
	$(CXX) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f src/*.o src/sdk/*.o $(HACK)

