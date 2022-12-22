#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace Vulkan {
struct Device {
  VkDevice device_;
  VkQueue graphicsQueue_;
  VkQueue presentQueue_;

  Device() {}
  ~Device() { vkDestroyDevice(device_, nullptr); }
  static std::shared_ptr<Device>
  CreateLogicalDevice(VkPhysicalDevice physicalDevice_, VkSurfaceKHR surface_,
                      const std::vector<const char *> &deviceExtensions);
  void Wait() { vkDeviceWaitIdle(device_); }
};

} // namespace Vulkan
