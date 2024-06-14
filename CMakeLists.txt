cmake_minimum_required(VERSION 3.8)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_C_EXTENSIONS OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

project(test VERSION 0.2 LANGUAGES C)

add_compile_options(-Wall -Wextra)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=incompatible-pointer-types")

add_executable(
    test
    "test.c"
)

if (CMAKE_BUILD_TYPE MATCHES "Asan")
    set(CMAKE_BUILD_TYPE "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined -fsanitize=address")
    target_link_options(test BEFORE PRIVATE -fsanitize=undefined PRIVATE -fsanitize=address)
endif()

message(STATUS "CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'")

set_property(TARGET test PROPERTY C_STANDARD 23)
target_compile_definitions(test PRIVATE "-DLOGS") # used for LOG_* macros
target_compile_definitions(test PRIVATE "-D_DEFAULT_SOURCE") # needed for reallocarray
