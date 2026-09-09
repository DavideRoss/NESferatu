BUILD_DIR := build
BUILD_TYPE := Debug

.PHONY: all configure build build-test test run clean

all: build

configure:
	cmake -S . -B $(BUILD_DIR) -G Ninja -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: configure
	cmake --build $(BUILD_DIR) --target NESferatu

build-test: configure
	cmake --build $(BUILD_DIR)

test: build-test
	ctest --test-dir $(BUILD_DIR) --output-on-failure

run: build
	./$(BUILD_DIR)/bin/NESferatu

clean:
	cmake --build $(BUILD_DIR) --target clean

release:
	$(MAKE) BUILD_TYPE=Release build
