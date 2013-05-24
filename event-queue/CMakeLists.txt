cmake_minimum_required(VERSION 2.6)

# projects include directory
include_directories(${ZEROMQ_INCLUDE_DIR})
include_directories(${CZEROMQ_INCLUDE_DIR})

add_executable(event_queue event_queue.c)
target_link_libraries(event_queue ${ZEROMQ_LIBRARIES})
target_link_libraries(event_queue ${CZEROMQ_LIBRARIES})
