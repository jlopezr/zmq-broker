cmake_minimum_required(VERSION 2.6)

# projects include directory
include_directories(${ZEROMQ_INCLUDE_DIR})
include_directories(${CZEROMQ_INCLUDE_DIR})

add_executable(ipc.broker broker.c)
target_link_libraries(ipc.broker ${ZEROMQ_LIBRARIES})
target_link_libraries(ipc.broker ${CZEROMQ_LIBRARIES})

add_executable(ipc.pub pub.c)
set_target_properties(ipc.pub PROPERTIES OUTPUT_NAME pub)
target_link_libraries(ipc.pub ${ZEROMQ_LIBRARIES})
target_link_libraries(ipc.pub ${CZEROMQ_LIBRARIES})

add_executable(ipc.pub2 pub2.c)
set_target_properties(ipc.pub2 PROPERTIES OUTPUT_NAME pub2)
target_link_libraries(ipc.pub2 ${ZEROMQ_LIBRARIES})
target_link_libraries(ipc.pub2 ${CZEROMQ_LIBRARIES})

add_executable(ipc.sub sub.c)
set_target_properties(ipc.sub PROPERTIES OUTPUT_NAME sub)
target_link_libraries(ipc.sub ${ZEROMQ_LIBRARIES})
target_link_libraries(ipc.sub ${CZEROMQ_LIBRARIES})
