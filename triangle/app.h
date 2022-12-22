#pragma once
#include <functional>
#include <vulkan/vulkan.h>

using GetSurface =
    std::function<VkSurfaceKHR(VkInstance, int *width, int *height)>;

class HelloTriangleApplication {
  class Impl *impl_ = nullptr;

public:
  HelloTriangleApplication(bool enableValidationLayers);
  ~HelloTriangleApplication();
  bool initialize(const char **extensions, size_t size,
                  const GetSurface &callback);
  void drawFrame();
};
