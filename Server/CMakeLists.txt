file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_executable(Server ${SOURCES})

target_include_directories(Server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(Server PRIVATE GameNetworkingSockets::GameNetworkingSockets)