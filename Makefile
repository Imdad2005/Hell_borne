SRC_DIR=../..
SDL2_DIR=$(HOME)/src/sdl/nto/sdl2_aarch64le/system/lib/cmake/SDL2/

patrace: build $(SRC_DIR)/CMakeLists.txt
	cd build; cmake \
	    -DCMAKE_TOOLCHAIN_FILE=../cmake-toolchain-qnx-aarch64le.cmake \
	    -DSDL2_DIR=$(SDL2_DIR) \
	    $(SRC_DIR)/..
	cd build; make

build:
	mkdir build

clean:
	rm -rf build
