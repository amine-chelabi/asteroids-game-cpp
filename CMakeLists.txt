cmake_minimum_required(VERSION 3.16)

project(AsteroidsGame VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_GAME "Build the SFML game" ON)
if(BUILD_GAME)
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

add_executable(asteroids src/main.cpp)

target_include_directories(asteroids PRIVATE include)

target_link_libraries(asteroids PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)

target_compile_options(asteroids PRIVATE
    -Wall
    -Wextra
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
endif()
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(test_collision tests/test_collision.cpp)
    target_include_directories(test_collision PRIVATE include)
    add_test(NAME collision COMMAND test_collision)
    add_executable(test_physics tests/test_physics.cpp)
    target_include_directories(test_physics PRIVATE include)
    add_test(NAME physics COMMAND test_physics)
endif()

option(BUILD_BENCHMARKS "Build performance benchmarks" OFF)
if(BUILD_BENCHMARKS)
    add_executable(benchmark_collision benchmarks/benchmark_collision.cpp)
    target_include_directories(benchmark_collision PRIVATE include)
endif()
