BUILD_DIR := build
BUILD_TYPE := Debug
SDL2_DIR := ~/dev/third-party/sdl2/SDL2-2.32.10/x86_64-w64-mingw32/lib/cmake/SDL2

.PHONY: all configure build build-win build-test test run clean

all: build

configure:
	cmake -S . -B $(BUILD_DIR)/linux -G Ninja \
	-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON

configure-win:
	cmake -S . -B $(BUILD_DIR)/win -G Ninja \
	-DCMAKE_TOOLCHAIN_FILE=windows-mingw64.cmake \
	-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
	-DSDL2_DIR=$(SDL2_DIR) \
	-DNESFERATU_BUILD_TESTS=OFF

build: configure
	cmake --build $(BUILD_DIR)/linux --target NESferatu -j32

build-win: configure-win
	cmake --build $(BUILD_DIR)/win --target NESferatu -j32
	cp "$$(x86_64-w64-mingw32-g++ -print-file-name=libstdc++-6.dll)" build/win/bin/
	cp "$$(x86_64-w64-mingw32-g++ -print-file-name=libgcc_s_seh-1.dll)" build/win/bin/

build-test: configure
	cmake --build $(BUILD_DIR)/tests -j32

test: build-test
	ctest --test-dir $(BUILD_DIR)/tests --output-on-failure

run: build
	./$(BUILD_DIR)/linux/bin/NESferatu
# 	 2>&1 | tee temp/nesferatu.log

clean:
	cmake --build $(BUILD_DIR) --target clean

release:
	$(MAKE) BUILD_TYPE=Release build
