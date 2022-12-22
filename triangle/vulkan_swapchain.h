#pragma once
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

namespace Vulkan {
struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
  static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device,
                                                       VkSurfaceKHR surface);
};
struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;
  static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device,
                                              VkSurfaceKHR surface);
  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface,
                             const std::vector<const char *> &deviceExtensions);

} // namespace Vulkan