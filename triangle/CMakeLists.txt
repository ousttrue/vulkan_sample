# https://cmake.org/cmake/help/latest/module/FindVulkan.html
# depend on $ENV{VULKAN_SDK}
find_package(Vulkan REQUIRED)

set(TARGET_NAME triangle)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE Vulkan::Vulkan)
install(TARGETS ${TARGET_NAME})
install(
  FILES $<TARGET_PDB_FILE:${TARGET_NAME}>
  DESTINATION bin
  OPTIONAL)
