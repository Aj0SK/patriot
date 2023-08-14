# https://devhints.io/makefile
CXX = g++
CXXFLAGS = -Wall -Wextra -O3 -march=native -std=c++17
SDLFLAGS = -lSDL2
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
	$(CXX) $(CXXFLAGS) -o $(BUILD)/app.out $(SRC)/main.cpp $(SDLFLAGS)