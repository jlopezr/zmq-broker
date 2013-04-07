cmake_minimum_required(VERSION 2.6)

# projects include directory
include_directories(${ZEROMQ_INCLUDE_DIR})
include_directories(${CZEROMQ_INCLUDE_DIR})

add_executable(inproc inproc.c)
target_link_libraries(inproc ${ZEROMQ_LIBRARIES})
target_link_libraries(inproc ${CZEROMQ_LIBRARIES})


add_executable(threads threads.c)
target_link_libraries(threads ${ZEROMQ_LIBRARIES})
target_link_libraries(threads ${CZEROMQ_LIBRARIES})
