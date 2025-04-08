BUILD_DIR := ./build
SRC_DIR := ./src

CC := gcc
CFLAGS := -std=c11

# .PHONY: db
db: $(BUILD_DIR)/db
	./build/db

$(BUILD_DIR)/db: $(BUILD_DIR)/main.o
	$(CC) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
