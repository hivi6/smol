BUILD_DIR := build
FINAL_BIN := $(BUILD_DIR)/smol

INC_DIR := include
H_FILES := $(shell find $(INC_DIR) -name '*.h')
SRC_DIR := src
C_FILES := $(shell find $(SRC_DIR) -name '*.c')

.PHONY: build
build: $(FINAL_BIN)
	@echo build complete

$(FINAL_BIN): $(C_FILES) $(H_FILES)
	mkdir -p $(BUILD_DIR)
	$(CC) -o $(FINAL_BIN) -I $(INC_DIR) $(C_FILES)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

