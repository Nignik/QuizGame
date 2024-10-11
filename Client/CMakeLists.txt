file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_executable(Client ${SOURCES})
target_include_directories(Client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(Client PRIVATE glfw glad::glad glm::glm OpenGL::GL imgui::imgui GameNetworkingSockets::GameNetworkingSockets)

add_custom_command(
    TARGET Client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/shaders
            ${CMAKE_BINARY_DIR}/bin/shaders
)