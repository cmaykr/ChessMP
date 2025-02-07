cmake_minimum_required(VERSION 3.13)

project(chessClient)
project(chessServer)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories(include)

add_executable(chessClient src/client.cpp src/gameServer.cpp src/gameClient.cpp src/piece.cpp)
target_link_libraries(chessClient PRIVATE SDL2::SDL2)
target_link_libraries(chessClient PRIVATE SDL2_image::SDL2_image)
add_executable(chessServer src/server.cpp src/gameServer.cpp src/piece.cpp)
