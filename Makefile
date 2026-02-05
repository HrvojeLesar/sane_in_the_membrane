default_target: debug
.PHONY: default_target

CMAKE_EXTRA_FLAGS =

cmake_debug:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug $(CMAKE_EXTRA_FLAGS)
.PHONY: cmake_debug

cmake_release:
	cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release $(CMAKE_EXTRA_FLAGS)
.PHONY: cmake_release

build_exec:
	make -C build -j 16
.PHONY: build_exec

build_exec_release:
	make -C build-release -j 16
.PHONY: build_exec

clean_build_exec:
	make clean -C build -j 16
.PHONY: clean_build_exec

debug: cmake_debug build_exec
.PHONY: debug

debug_asan: CMAKE_EXTRA_FLAGS = -DWITH_ASAN=ON
debug_asan: cmake_debug build_exec
.PHONY: debug_asan

debug_clean: cmake_debug clean_build_exec
.PHONY: debug_clean

release: cmake_release build_exec_release
.PHONY: release

release_clean: cmake_release clean_build_exec
.PHONY: release_clean

clean:
	@rm -rf build
.PHONY: clean
