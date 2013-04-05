cmake_minimum_required(VERSION 2.6)

# projects include directory
include_directories(${ZEROMQ_INCLUDE_DIR})
include_directories(${CZEROMQ_INCLUDE_DIR})

add_executable(epgm.pub pub.c)
set_target_properties(epgm.pub PROPERTIES OUTPUT_NAME pub)
target_link_libraries(epgm.pub ${ZEROMQ_LIBRARIES})
target_link_libraries(epgm.pub ${CZEROMQ_LIBRARIES})

add_executable(epgm.sub sub.c)
set_target_properties(epgm.sub PROPERTIES OUTPUT_NAME sub)
target_link_libraries(epgm.sub ${ZEROMQ_LIBRARIES})
target_link_libraries(epgm.sub ${CZEROMQ_LIBRARIES})
