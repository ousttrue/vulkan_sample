#pragma once
#include <vector>
#include <vulkan/vulkan.h>

extern const bool enableValidationLayers;

class HelloTriangleApplication {
  class Impl *impl_ = nullptr;

public:
  HelloTriangleApplication();
  ~HelloTriangleApplication();
  VkInstance initVulkan(const std::vector<const char *> &extensions);
  void createSwapChain(VkSurfaceKHR surface, int width, int height);
  void drawFrame();
};
