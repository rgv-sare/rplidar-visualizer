cmake_minimum_required(VERSION 3.22)

# --- ImGui --- #

project(imgui)

add_library(${PROJECT_NAME}
    ./imgui/imgui.cpp
    ./imgui/imgui_demo.cpp
    ./imgui/imgui_draw.cpp
    ./imgui/imgui_tables.cpp
    ./imgui/imgui_widgets.cpp
    
    ./imgui/backends/imgui_impl_glfw.cpp
    ./imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ./imgui
    ./imguibackends
    ./glfw/include/
)

# --- RPLIDAR SDK --- #

project(rplidar_sdk)

file(GLOB_RECURSE RPLIDAR_SRC "rplidar_sdk/sdk/src/*.cpp")

if(UNIX AND NOT APPLE)
    file(GLOB_RECURSE WIN_SOURCES "rplidar_sdk/sdk/src/arch/win32/*.cpp")
    file(GLOB_RECURSE MAC_SOURCES "rplidar_sdk/sdk/src/arch/macOS/*.cpp")
    list(REMOVE_ITEM RPLIDAR_SRC ${WIN_SOURCES})
    list(REMOVE_ITEM RPLIDAR_SRC ${MAC_SOURCES})
elseif(APPLE)
    file(GLOB_RECURSE WIN_SOURCES "rplidar_sdk/sdk/src/arch/win32/*.cpp")
    file(GLOB_RECURSE UNIX_SOURCES "rplidar_sdk/sdk/src/arch/linux/*.cpp")
    list(REMOVE_ITEM RPLIDAR_SRC ${WIN_SOURCES})
    list(REMOVE_ITEM RPLIDAR_SRC ${UNIX_SOURCES})
else()
    file(GLOB_RECURSE UNIX_SOURCES "rplidar_sdk/sdk/src/arch/linux/*.cpp")
    file(GLOB_RECURSE MAC_SOURCES "rplidar_sdk/sdk/src/arch/macOS/*.cpp")
    list(REMOVE_ITEM RPLIDAR_SRC ${UNIX_SOURCES})
    list(REMOVE_ITEM RPLIDAR_SRC ${MAC_SOURCES})
endif()


add_library(${PROJECT_NAME} ${RPLIDAR_SRC})

target_include_directories(${PROJECT_NAME} PUBLIC
    ./rplidar_sdk/sdk/include
    ./rplidar_sdk/sdk/src
)