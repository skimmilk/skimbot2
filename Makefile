CXX=g++
CC=gcc
CFLAGS=-c -Wall -Wextra -g -fpic -m32 -O0 -std=c++11 -I src/ -Wfatal-errors -DPOSIX
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
	src/spin.cpp
OBJECTS=$(SOURCES:.cpp=.o)
HACK=libskimbot.so

all: $(HACK)

$(HACK): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f src/*.o src/sdk/*.o $(HACK)

