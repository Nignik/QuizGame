file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_executable(ServerCpp ${SOURCES})

target_include_directories(ServerCpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(ServerCpp PRIVATE GameNetworkingSockets::GameNetworkingSockets)