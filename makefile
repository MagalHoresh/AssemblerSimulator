TARGET := assembler

SRC_DIR := src
OBJ_DIR := obj
TESTS_DIR := tests

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CPPFLAGS := -Iinclude
CFLAGS   := -Wall -ansi -pedantic
LDFLAGS  := -Llib
LDLIBS   := -lm

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir $@

clean:
	$(RM) $(OBJ)
	-rm -rf *.o $(TESTS_DIR)/*.ob $(TESTS_DIR)/*.ent $(TESTS_DIR)/*.ext
	-rm -rf $(TARGET)