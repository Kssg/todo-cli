CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = todo

SRC = src/main.c src/todo.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC)

clean:
	rm -f $(TARGET)
