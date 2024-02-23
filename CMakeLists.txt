cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)

project(rplidar-visulizer)

add_subdirectory("dep/glfw")
add_subdirectory("dep/glad")
add_subdirectory("dep/glm")
add_subdirectory("dep/lua")
add_subdirectory("dep/googletest")
add_subdirectory("dep")

set(PROJECT_SOURCES
  src/main.cpp
  src/Visualizer.cpp
  src/Logger.cpp
  src/Input.cpp
  src/SceneObject.cpp
  src/Camera.cpp
  src/MeshObject.cpp
  src/MeshBuilder.cpp
  src/VisualizerScene.cpp
  src/Mesh.cpp
  src/stb_image.cpp
  src/LightObject.cpp
  src/Framebuffer.cpp
  src/LuaIndexable.cpp
  src/LIDARFramePreview.cpp
  src/LIDARFrameGrabber.cpp

  src/shaders/Shader.cpp
  src/shaders/PhongShader.cpp
  src/shaders/Compositor.cpp

  src/animation/Smoother.cpp
  src/animation/Easing.cpp
  src/animation/Track.cpp
  src/animation/Timeline.cpp
)

set(PROJECT_INCLUDES
  include
  dep/glfw/include
  dep/glad/include
  dep/glm
  dep/imgui
  dep/imgui/backends
  dep/rplidar_sdk/sdk/include
)

set(PROJECT_LIBRARIES
  glfw
  glad
  glm
  lua
  imgui
  rplidar_sdk
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDES})

target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBRARIES})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/res
  ${CMAKE_CURRENT_BINARY_DIR}/res
  COMMENT "Copying resources to build directory"
)

enable_testing()

set(TESTER ${PROJECT_NAME}-test)

list(REMOVE_ITEM PROJECT_SOURCES src/main.cpp)

add_executable(${TESTER}
  ${PROJECT_SOURCES}

  tests/test.cpp
  tests/luatests.cpp
  tests/animationtests.cpp
)

target_include_directories(${TESTER} PRIVATE ${PROJECT_INCLUDES})

target_link_libraries(${TESTER}
  ${PROJECT_LIBRARIES}

  gtest
)

add_test(NAME ${TESTER} COMMAND ${TESTER})

add_custom_command(TARGET ${TESTER} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/res
  ${CMAKE_CURRENT_BINARY_DIR}/res
  COMMENT "Copying resources to build directory"
)