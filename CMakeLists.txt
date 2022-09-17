cmake_minimum_required(VERSION 3.22)

project(test-mmap)

add_executable(test-mmap prueba.c)

target_link_libraries(test-mmap commons)
