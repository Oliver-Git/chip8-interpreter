# CMakeLists.txt : Contains instructions for the CMake build system.

cmake_minimum_required(VERSION 3.13)
project(Chip8_Interpreter LANGUAGES C)

set(CMAKE_C_STANDARD 11)

set(SOURCES
	src/chip8.c
	src/display.c
	src/main.c
	include/chip8.h
	include/display.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

if (WIN32)
	target_sources(
		${PROJECT_NAME} PRIVATE
		src/windows_sleep.c
		include/windows_sleep.h
	)
endif (WIN32)

# Find SDL2 installation. This will only work if FindPkgConfig is available.
# To build on Windows, an environment that provides FindPkgConfig may be required,
# such as MSYS2 or Cygwin.

# I originally tried to use someone else's FindSDL2.cmake module,
# however it unfortunately did not work on Windows.

include(FindPkgConfig)

pkg_search_module(SDL2 REQUIRED sdl2)

include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
