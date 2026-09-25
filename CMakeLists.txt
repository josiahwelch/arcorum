cmake_minimum_required(VERSION 3.15)
project(arcorum C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

add_executable(arcorum
    src/arcorum.c
    src/lexer.c
        include/ast.h
)

target_include_directories(arcorum PRIVATE
    include
    /usr/local/include
)

target_link_directories(arcorum PRIVATE /usr/local/lib)
