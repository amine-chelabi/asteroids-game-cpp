cmake_minimum_required(VERSION 3.16)

project(AsteroidsGame VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

include(CTest)
if(BUILD_TESTING)
    add_executable(test_collision tests/test_collision.cpp)
    target_include_directories(test_collision PRIVATE include)
    add_test(NAME collision COMMAND test_collision)
endif()
