cmake_minimum_required(VERSION 3.20)

# use this set if outside visual studio to set toolchain
#set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a .lib)
set(BUILD_SHARED_LIBS OFF)


project(QuizGame)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(GameNetworkingSockets CONFIG REQUIRED)

add_subdirectory("Client")
add_subdirectory("ServerCpp")
