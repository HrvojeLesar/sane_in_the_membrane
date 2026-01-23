default_target: debug
.PHONY: default_target

CMAKE_EXTRA_FLAGS =

cmake_debug:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug $(CMAKE_EXTRA_FLAGS)
.PHONY: cmake_debug

cmake_release:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Release $(CMAKE_EXTRA_FLAGS)
.PHONY: cmake_release

build_exec:
	make -C build -j 16
.PHONY: build_exec

clean_build_exec:
	make clean -C build -j 16
.PHONY: clean_build_exec

debug: cmake_debug build_exec
.PHONY: debug

debug_skip_gui: CMAKE_EXTRA_FLAGS = -DSKIP_GUI=ON
debug_skip_gui: debug
.PHONY: debug_skip_gui

debug_skip_server: CMAKE_EXTRA_FLAGS = -DSKIP_SERVER=ON
debug_skip_server: debug
.PHONY: debug_skip_server

debug_clean: cmake_debug clean_build_exec
.PHONY: debug_clean

release: cmake_release build_exec
.PHONY: release

release_skip_gui: CMAKE_EXTRA_FLAGS = -DSKIP_GUI=ON
release_skip_gui: release
.PHONY: release_skip_gui

release_skip_server: CMAKE_EXTRA_FLAGS = -DSKIP_SERVER=ON
release_skip_server: release
.PHONY: release_skip_server

release_clean: cmake_release clean_build_exec
.PHONY: release_clean

clean:
	@rm -rf build
.PHONY: clean
