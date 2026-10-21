# Сборка C++ API для Linux
CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -I"src" -I"include"
LDFLAGS  = -lltrapi -lltr24api -lpthread

SRC = src/main.cpp src/config.cpp src/server.cpp
BIN = test

all: $(BIN)

$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN) $(LDFLAGS)

clean:
	rm -f $(BIN)

.PHONY: all clean