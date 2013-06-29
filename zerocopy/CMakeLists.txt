cmake_minimum_required(VERSION 2.6)

# projects include directory
include_directories(${ZEROMQ_INCLUDE_DIR})
include_directories(${CZEROMQ_INCLUDE_DIR})

add_executable(tcp.broker broker.c)
target_link_libraries(tcp.broker ${ZEROMQ_LIBRARIES})
target_link_libraries(tcp.broker ${CZEROMQ_LIBRARIES})

add_executable(tcp.pub pub.c)
set_target_properties(tcp.pub PROPERTIES OUTPUT_NAME pub)
target_link_libraries(tcp.pub ${ZEROMQ_LIBRARIES})
target_link_libraries(tcp.pub ${CZEROMQ_LIBRARIES})

add_executable(tcp.pub2 pub2.c)
set_target_properties(tcp.pub2 PROPERTIES OUTPUT_NAME pub2)
target_link_libraries(tcp.pub2 ${ZEROMQ_LIBRARIES})
target_link_libraries(tcp.pub2 ${CZEROMQ_LIBRARIES})

add_executable(tcp.sub sub.c)
set_target_properties(tcp.sub PROPERTIES OUTPUT_NAME sub)
target_link_libraries(tcp.sub ${ZEROMQ_LIBRARIES})
target_link_libraries(tcp.sub ${CZEROMQ_LIBRARIES})
