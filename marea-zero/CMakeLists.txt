cmake_minimum_required(VERSION 2.6)

# projects include directory
include_directories(${ZEROMQ_INCLUDE_DIR})
include_directories(${CZEROMQ_INCLUDE_DIR})

add_executable(mz.pub pub.c)
set_target_properties(mz.pub PROPERTIES OUTPUT_NAME pub)
target_link_libraries(mz.pub ${ZEROMQ_LIBRARIES})
target_link_libraries(mz.pub ${CZEROMQ_LIBRARIES})

add_executable(mz.sub sub.c)
set_target_properties(mz.sub PROPERTIES OUTPUT_NAME sub)
target_link_libraries(mz.sub ${ZEROMQ_LIBRARIES})
target_link_libraries(mz.sub ${CZEROMQ_LIBRARIES})
