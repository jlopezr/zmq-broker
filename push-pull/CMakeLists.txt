cmake_minimum_required(VERSION 2.8)

# projects include directory
include_directories(${ZEROMQ_INCLUDE_DIR})
include_directories(${CZEROMQ_INCLUDE_DIR})

add_executable(taskvent taskvent.c)
target_link_libraries(taskvent ${ZEROMQ_LIBRARIES})
target_link_libraries(taskvent ${CZEROMQ_LIBRARIES})

add_executable(taskwork taskwork.c)
target_link_libraries(taskwork ${ZEROMQ_LIBRARIES})
target_link_libraries(taskwork ${CZEROMQ_LIBRARIES})

add_executable(taskwork2 taskwork2.c)
target_link_libraries(taskwork2 ${ZEROMQ_LIBRARIES})
target_link_libraries(taskwork2 ${CZEROMQ_LIBRARIES})

add_executable(tasksink tasksink.c)
target_link_libraries(tasksink ${ZEROMQ_LIBRARIES})
target_link_libraries(tasksink ${CZEROMQ_LIBRARIES})
