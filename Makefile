CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./include -I.
LDFLAGS = -lm

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
TARGET = $(BIN_DIR)/logistic_network

.PHONY: all clean directories test_json

all: directories $(TARGET)

directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

run: all
	./$(TARGET)

debug: CFLAGS += -g
debug: clean all

# Règle pour compiler directement avec gcc
compile:
	$(CC) $(CFLAGS) $(SRC_DIR)/main.c -o $(BIN_DIR)/logistic_network $(LDFLAGS)

# Règle pour compiler le test JSON
test_json: $(OBJ_DIR)/test_json.o $(OBJ_DIR)/json_handler.o
	$(CC) $(LDFLAGS) -o $(BIN_DIR)/test_json $^

$(OBJ_DIR)/test_json.o: src/test_json.c
	$(CC) $(CFLAGS) -c $< -o $@