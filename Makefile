# https://devhints.io/makefile
CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17
SDL_FLAGS = -lSDL2
SRC = src
BUILD = build

all: run

run: app
	$(BUILD)/$<.out

clean:
	rm -r -f $(BUILD)

prepare:
	mkdir -p $(BUILD)

app: prepare $(SRC)/main.cpp
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -o $(BUILD)/app.out $(SRC)/main.cpp $(SDL_FLAGS)