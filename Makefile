CXX=g++
CXXFLAGS=-std=c++20 -Wall -Wextra -O3
#CXXFLAGS=-std=c++20 -Wall -Wextra -g
OBJS=main.o Game.o State.o Piece.o Common.o
HEADERS=Common.h Piece.h State.h Game.h

all: asteroid

asteroid: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o AsteroidEscape.exe

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) AsteroidEscape.exe

.PHONY: all asteroid clean
