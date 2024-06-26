cmake_minimum_required(VERSION 3.8)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

project(test VERSION 0.2 LANGUAGES C)

add_compile_options(-Wall -Wextra -Werror=incompatible-pointer-types)
add_compile_definitions("-DLOGS") # used for LOG_* CERR*/COUT* macros
add_compile_definitions("-D_DEFAULT_SOURCE") # needed for reallocarray

# add_executable(
#     test
#     "test.c"
# )

add_executable(
    test2
    "test2.c"
)

# add_executable(
#     test3
#     "test3.c"
# )

find_library(MATH m REQUIRED)
# target_link_libraries(test PRIVATE m)
target_link_libraries(test2 PRIVATE m)
# target_link_libraries(test3 PRIVATE m)

if (CMAKE_BUILD_TYPE MATCHES "Asan")
    set(CMAKE_BUILD_TYPE "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined -fsanitize=address")
    add_link_options(-fsanitize=undefined -fsanitize=address)
endif()

message(STATUS "CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'")
