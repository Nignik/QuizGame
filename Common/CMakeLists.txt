cmake_minimum_required(VERSION 3.20)

if (WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_FIND_LIBRARY_SUFFIXES .a .lib)
	set(BUILD_SHARED_LIBS OFF)
endif()

project(Common)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

find_package(Protobuf CONFIG REQUIRED)
file(GLOB PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.proto")
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

add_library(Common STATIC ${SOURCES} ${HEADERS} ${PROTO_SRCS} ${PROTO_HDRS})

include_directories(${PROTOBUF_INCLUDE_DIRS})
target_link_libraries(Common PRIVATE protobuf::libprotobuf)

target_include_directories(Common PUBLIC
    ${PROTOBUF_INCLUDE_DIRS}
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include/Common>)

install(TARGETS Common EXPORT CommonTargets
        ARCHIVE DESTINATION lib           # For static libraries
        LIBRARY DESTINATION lib           # For shared libraries
        RUNTIME DESTINATION bin)          # For executables

# Export the target for external use
install(EXPORT CommonTargets
        FILE CommonConfig.cmake
        NAMESPACE Common::                # Use Common:: as a namespace prefix for this library
        DESTINATION lib/cmake/Common)   

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/
        DESTINATION include/Common
        FILES_MATCHING PATTERN "*.h")