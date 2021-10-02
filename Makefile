BUILD_DIR := build
SRC_DIR := src
SCRIPT_DIR := scripts

CC_FLAGS := 


all: $(BUILD_DIR) $(BUILD_DIR)/payload $(BUILD_DIR)/init_hook $(BUILD_DIR)/main

$(BUILD_DIR):
	mkdir $(BUILD_DIR) 2>/dev/null

$(BUILD_DIR)/payload: $(SRC_DIR)/payload.c
	cc $(CC_FLAGS) -O3 -nostartfiles -s $(SRC_DIR)/payload.c -o $(BUILD_DIR)/payload

$(BUILD_DIR)/payload_defs.c: $(BUILD_DIR)/payload
	python3 $(SCRIPT_DIR)/create_def_file.py payload $(BUILD_DIR)/payload > $(BUILD_DIR)/payload_defs.c

$(BUILD_DIR)/init_hook: $(BUILD_DIR)/payload_defs.c $(SRC_DIR)/init_hook.c
	cc $(CC_FLAGS) $(BUILD_DIR)/payload_defs.c $(SRC_DIR)/init_hook.c -o $(BUILD_DIR)/init_hook

$(BUILD_DIR)/init_hook_defs.c: $(BUILD_DIR)/init_hook
	python3 $(SCRIPT_DIR)/create_def_file.py init_hook $(BUILD_DIR)/init_hook > $(BUILD_DIR)/init_hook_defs.c

$(BUILD_DIR)/main: $(BUILD_DIR)/init_hook_defs.c $(SRC_DIR)/main.c
	cc $(CC_FLAGS) $(BUILD_DIR)/init_hook_defs.c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main

clean:
	rm -r $(BUILD_DIR)/