cmake_minimum_required(VERSION 2.6)

# projects include directory
include_directories(${ZEROMQ_INCLUDE_DIR})
include_directories(${CZEROMQ_INCLUDE_DIR})

add_executable(copy-pub copy-pub.c)
target_link_libraries(copy-pub ${ZEROMQ_LIBRARIES})
target_link_libraries(copy-pub ${CZEROMQ_LIBRARIES})

add_executable(copy-sub copy-sub.c)
target_link_libraries(copy-sub ${ZEROMQ_LIBRARIES})
target_link_libraries(copy-sub ${CZEROMQ_LIBRARIES})

add_executable(zerocopy-pub zerocopy-pub.c)
target_link_libraries(zerocopy-pub ${ZEROMQ_LIBRARIES})
target_link_libraries(zerocopy-pub ${CZEROMQ_LIBRARIES})

add_executable(zerocopy-sub zerocopy-sub.c)
target_link_libraries(zerocopy-sub ${ZEROMQ_LIBRARIES})
target_link_libraries(zerocopy-sub ${CZEROMQ_LIBRARIES})


