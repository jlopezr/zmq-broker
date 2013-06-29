cmake_minimum_required(VERSION 2.6)

# projects include directory
include_directories(${ZEROMQ_INCLUDE_DIR})
include_directories(${CZEROMQ_INCLUDE_DIR})

add_executable(test01 test01.c)

add_executable(test02 test02.c)

add_executable(test03 test03.c)


