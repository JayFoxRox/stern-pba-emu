cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)

include_directories(SYSTEM
  ${SDL2_INCLUDE_DIR}
)

add_executable(stern-pba-emu main.c)
target_link_libraries(stern-pba-emu ${SDL2_LIBRARY})