snakegame: main.o game.o snake.o
	g++ -o snakegame main.o game.o snake.o -lcurses -lSDL2 -lSDL2_ttf -lSDL2_mixer
main.o: main.cpp game.h
	g++ -c main.cpp
game.o: game.cpp snake.h constants.h
	g++ -c game.cpp
snake.o: snake.cpp constants.h
	g++ -c snake.cpp
clean:
	rm *.o 
	rm snakegame
	rm record.dat
