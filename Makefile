BUILD_DIR := build
SRC_DIR := src
INC_DIR := include

CC := clang
CFLAGS := -g -std=c11

OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))

run: $(BUILD_DIR)/db
	./$(BUILD_DIR)/db

debug: $(BUILD_DIR)/db
	lldb ./$(BUILD_DIR)/db

.PHONY: db
db: $(BUILD_DIR)/db

$(BUILD_DIR)/db: $(OBJS)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(BUILD_DIR)
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
