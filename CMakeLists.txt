cmake_minimum_required(VERSION 2.6)
project(zmq-broker)

# additional cmake modules location path
set(
  CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules
  CACHE
  PATH "CMake modules search path"
)

find_package(ZMQ)
find_package(CZMQ)

add_subdirectory(ipc)
add_subdirectory(tcp)
#add_subdirectory(epgm)
add_subdirectory(beacon)
add_subdirectory(push-pull)
add_subdirectory(inproc)
add_subdirectory(marea-zero)
add_subdirectory(reactor)
add_subdirectory(socket-monitor)
add_subdirectory(event-queue)
add_subdirectory(nanoseconds)
add_subdirectory(rwlock)
add_subdirectory(zerocopy)

