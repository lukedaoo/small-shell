CC = gcc
CFLAGS = -Wall -Wextra
TARGET = mysh
SRC = mysh.c
TEST_TARGET = test_mysh
TEST_SRC = test_mysh.c

.PHONY: all compile run clean test

all: clean compile run

compile:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run:
	./$(TARGET)

test:
	$(CC) $(CFLAGS) $(TEST_SRC) $(SRC) -o $(TEST_TARGET) -DTEST_MODE
	./$(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET)
