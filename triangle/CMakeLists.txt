# https://cmake.org/cmake/help/latest/module/FindVulkan.html
#
# depend on $ENV{VULKAN_SDK}
find_package(Vulkan REQUIRED)

include(FetchContent)

# Use static libraries
set(BUILD_SHARED_LIBS ON)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.3.8)
# FetchContent_Declare( glm GIT_REPOSITORY https://github.com/g-truc/glm.git
# GIT_TAG 0.9.9.8)
FetchContent_MakeAvailable(glfw)

set(TARGET_NAME triangle)
add_executable(${TARGET_NAME} main.cpp app.cpp vulkan_instance.cpp
                              vulkan_swapchain.cpp)
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 20)
target_link_libraries(${TARGET_NAME} PRIVATE glfw Vulkan::Vulkan)
install(TARGETS ${TARGET_NAME})
install(
  FILES $<TARGET_PDB_FILE:${TARGET_NAME}>
  DESTINATION bin
  OPTIONAL)
