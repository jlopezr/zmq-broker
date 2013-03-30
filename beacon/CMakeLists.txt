cmake_minimum_required(VERSION 2.8)

# projects include directory
include_directories(${ZEROMQ_INCLUDE_DIR})
include_directories(${CZEROMQ_INCLUDE_DIR})

add_executable(beacon beacon.c)
target_link_libraries(beacon ${ZEROMQ_LIBRARIES})
target_link_libraries(beacon ${CZEROMQ_LIBRARIES})
