cmake_minimum_required(VERSION 3.20)

if (WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_FIND_LIBRARY_SUFFIXES .a .lib)
	set(BUILD_SHARED_LIBS OFF)
endif()

project(Server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GameNetworkingSockets CONFIG REQUIRED)
find_package(Common REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_executable(Server ${SOURCES})

target_include_directories(Server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(Server PRIVATE GameNetworkingSockets::GameNetworkingSockets Common::Common protobuf::libprotobuf yaml-cpp::yaml-cpp)