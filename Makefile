# Define the build directory
BUILD_DIR = build
BUILD_TYPE ?= Debug  # Default to 'Debug' if BUILD_TYPE is not defined
# Default target Linux
all: configure build run

configure-windows:
	@mkdir $(BUILD_DIR)> NUL
	@cmake build . -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=${BUILD_TYPE}

configure:
	@mkdir -p $(BUILD_DIR)
	@cmake build . -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=${BUILD_TYPE}

# Build the project
build:
	@cd $(BUILD_DIR) && $(MAKE)

run:
	./$(BUILD_DIR)/raylib-template/raylib-template

.PHONY: all configure build run