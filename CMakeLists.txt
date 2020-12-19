cmake_minimum_required(VERSION 3.19.2)

project(Program)

file(GLOB_RECURSE program_sources CONFIGURE_DEPENDS "src/*.h" "src/*.cpp")

add_executable(${PROJECT_NAME} ${program_sources})

include_directories(P:/sdks/glfw-3.3.2.bin.WIN64/include)
include_directories(P:/sdks/VulkanSDK/1.2.162.0/Include)

target_link_libraries(${PROJECT_NAME} -LP:/sdks/VulkanSDK/1.2.162.0/Lib)
target_link_libraries(${PROJECT_NAME} -LP:/sdks/glfw-3.3.2.bin.WIN64/lib-vc2019)
target_link_libraries(${PROJECT_NAME} -lvulkan-1)
target_link_libraries(${PROJECT_NAME} -lglfw3dll)