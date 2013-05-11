cmake_minimum_required(VERSION 2.6)

# projects include directory
include_directories(${ZEROMQ_INCLUDE_DIR})
include_directories(${CZEROMQ_INCLUDE_DIR})

add_executable(monitor monitor.c)
target_link_libraries(monitor ${ZEROMQ_LIBRARIES})
target_link_libraries(monitor ${CZEROMQ_LIBRARIES})

