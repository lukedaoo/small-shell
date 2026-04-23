CC = gcc
CFLAGS = -Wall -Wextra
TARGET = mysh
SRC = mysh.c

.PHONY: all compile run clean

all: clean compile run

compile:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run:
	./$(TARGET)

clean:
	rm -f $(TARGET)
