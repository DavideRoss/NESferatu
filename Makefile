BUILD_DIR := build
BUILD_TYPE := Debug

.PHONY: all configure build build-test test run clean

all: build

configure:
	cmake -S . -B $(BUILD_DIR) -G Ninja -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: configure
	cmake --build $(BUILD_DIR) --target NESferatu -j32

build-test: configure
	cmake --build $(BUILD_DIR) -j32

test: build-test
	ctest --test-dir $(BUILD_DIR) --output-on-failure

run: build
	./$(BUILD_DIR)/bin/NESferatu 2>&1 | tee temp/nesferatu.log

clean:
	cmake --build $(BUILD_DIR) --target clean

release:
	$(MAKE) BUILD_TYPE=Release build
