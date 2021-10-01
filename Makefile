BUILD_DIR := build
SRC_DIR := src


all: $(BUILD_DIR) $(BUILD_DIR)/payload.o $(BUILD_DIR)/payload

$(BUILD_DIR):
	mkdir $(BUILD_DIR) 2>/dev/null

$(BUILD_DIR)/payload.o: $(SRC_DIR)/payload.c
	cc -c $(SRC_DIR)/payload.c -o $(BUILD_DIR)/payload.o

$(BUILD_DIR)/payload: $(BUILD_DIR)/payload.o
	cc $(BUILD_DIR)/payload.o -o $(BUILD_DIR)/payload

clean:
	rm -r $(BUILD_DIR)/