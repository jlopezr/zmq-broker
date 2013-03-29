cmake_minimum_required(VERSION 2.8)

add_executable(broker broker.c)
#add_executable(sub sub.c)
#add_executable(pub pub.c)
#add_executable(pub2 pub2.c)

find_library(ZMQ zmq)
include_directories(${PNG_INCLUDE_DIR})
target_link_libraries(broker ${PNG_LIBRARY})

find_library(CZMQ czmq)
include_directories(${CZMQ_INCLUDE_DIR})
target_link_libraries(broker ${PNG_LIBRARY})


