CC := clang
CFLAGS := -std=c11 -g -Wall -Werror

BUILD_DIR := build
SRC_DIRS := src external/c-core/src
INC_DIRS := include external/c-core/include

vpath %.c $(SRC_DIRS)

I_FLAGS := $(foreach dir, $(INC_DIRS), -I$(dir))

OBJS := $(foreach dir, $(SRC_DIRS), $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(wildcard $(dir)/*.c))))

run: db
	./$(BUILD_DIR)/db

debug: db
	lldb ./$(BUILD_DIR)/db

.PHONY: db
db: $(BUILD_DIR)/db

$(BUILD_DIR)/db: $(OBJS)
	$(CC) $^ -o $@

$(BUILD_DIR)/%.o: %.c $(BUILD_DIR)
	$(CC) $(CFLAGS) $(I_FLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

.PHONY: init
init:
	make clean
	bear -- make db

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
