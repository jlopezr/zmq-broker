cmake_minimum_required(VERSION 2.6)

# projects include directory
include_directories(${ZEROMQ_INCLUDE_DIR})
include_directories(${CZEROMQ_INCLUDE_DIR})

add_executable(reactor reactor.c)
target_link_libraries(reactor ${ZEROMQ_LIBRARIES})
target_link_libraries(reactor ${CZEROMQ_LIBRARIES})
