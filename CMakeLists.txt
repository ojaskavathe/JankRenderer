cmake_minimum_required(VERSION 3.7)

project(JankRenderer)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/glfw ${CMAKE_BINARY_DIR}/external/glfw)
add_subdirectory(${CMAKE_SOURCE_DIR}/external/glm ${CMAKE_BINARY_DIR}/external/glm)


file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/openGL/src/*.cpp" ${CMAKE_SOURCE_DIR}/external/glad/src/gl.c)

add_executable(GAME ${SRC_FILES})
target_link_libraries(GAME
    PRIVATE OpenGL::GL
    PRIVATE glfw
    PRIVATE glm)

target_include_directories(GAME 
    PUBLIC ${CMAKE_SOURCE_DIR}/external/glfw/include
    PUBLIC ${CMAKE_SOURCE_DIR}/external/glad/include
    PUBLIC ${CMAKE_SOURCE_DIR}/external/glm
    PUBLIC ${CMAKE_SOURCE_DIR}/external/json
    PUBLIC ${CMAKE_SOURCE_DIR}/openGL/src
    PUBLIC ${CMAKE_SOURCE_DIR}/openGL/src/vendor)

add_custom_command(
    TARGET GAME POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
      ${CMAKE_SOURCE_DIR}/openGL/res
      $<TARGET_FILE_DIR:GAME>/res
    COMMENT "Copying res to output directory: ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}")