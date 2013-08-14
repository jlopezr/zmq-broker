cmake_minimum_required(VERSION 2.6)

# Pthreads
find_package(Threads REQUIRED)

# projects include directory
include_directories(${ZEROMQ_INCLUDE_DIR})
include_directories(${CZEROMQ_INCLUDE_DIR})

add_executable(test01 test01.c)
target_link_libraries (test01 ${CMAKE_THREAD_LIBS_INIT} )

add_executable(test02 test02.c)
target_link_libraries (test02 ${CMAKE_THREAD_LIBS_INIT} )

add_executable(test03 test03.c)
target_link_libraries (test03 ${CMAKE_THREAD_LIBS_INIT} )

add_executable(create_pthreads create_pthreads.c)
target_link_libraries (create_pthreads ${CMAKE_THREAD_LIBS_INIT} )
