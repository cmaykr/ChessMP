cmake_minimum_required(VERSION 3.13)

project(chessClient)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(chessClient client.cpp piece.cpp gameClient.cpp game.cpp)
target_link_libraries(chessClient PRIVATE SDL2::SDL2)
target_link_libraries(chessClient PRIVATE SDL2_image::SDL2_image)
