all:
	gcc recognizer.c main.c -o recognizer -Wall -Wextra -lSDL2 -lm
	gcc recognizer.c generator.c -o generator -Wall -Wextra -lSDL2 -lm

clean:
	rm -f recognizer
	rm -f generator
