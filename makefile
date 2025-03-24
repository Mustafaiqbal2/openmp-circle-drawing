CC = g++
CFLAGS = -Wall -O3 -fopenmp
LDFLAGS = -lSDL2

all: circle_drawing

circle_drawing: circle_drawing.cpp
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f circle_drawing

.PHONY: all clean