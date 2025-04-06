CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2
INCLUDE = -Iinclude
SRC = src/main.c
OBJ = $(SRC:.c=.o)
TARGET = logistic_network

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean