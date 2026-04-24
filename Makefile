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
	$(CC) $(CFLAGS) -c $(SRC) -Dmain=_mysh_main -o mysh_test.o
	$(CC) $(CFLAGS) $(TEST_SRC) mysh_test.o -o $(TEST_TARGET)
	rm -f mysh_test.o
	./$(TEST_TARGET)

test-binary:
	$(CC) $(CFLAGS) -c $(SRC) -o $(TARGET)
	bash test_mysh_binary.sh

clean:
	rm -f $(TARGET) $(TEST_TARGET)
