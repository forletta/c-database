BUILD_DIR := build
SRC_DIR := src
INC_DIR := include

CC := gcc
CFLAGS := -std=c11

OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))

db: $(BUILD_DIR)/db
	./$(BUILD_DIR)/db

$(BUILD_DIR)/db: $(OBJS)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
