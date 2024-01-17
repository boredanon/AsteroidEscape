CXX=g++
CXXFLAGS= -g	-Wall	-Wextra	-O3	-msse	-msse2
#CXXFLAGS=	-Wall	-Wextra	-g
OBJS=	main.o	Game.o	State.o	Piece.o	Common.o

all:	asteroid
asteroid:	$(OBJS)
	$(CXX)	$(CXXFLAGS)	$(OBJS)	-o	AsteroidEscape.exe
clean:
	rm	-f	$(OBJS)	AsteroidEscape.exe