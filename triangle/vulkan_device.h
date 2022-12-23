#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace Vulkan {
struct Device {
  VkDevice device_;
  VkQueue graphicsQueue_;
  VkQueue presentQueue_;
  VkSemaphore imageAvailableSemaphore_;
  VkSemaphore renderFinishedSemaphore_;
  VkFence inFlightFence_;

  Device() {}
  ~Device() {
    vkDestroySemaphore(device_, renderFinishedSemaphore_, nullptr);
    vkDestroySemaphore(device_, imageAvailableSemaphore_, nullptr);
    vkDestroyFence(device_, inFlightFence_, nullptr);
    vkDestroyDevice(device_, nullptr);
  }
  static std::shared_ptr<Device>
  CreateLogicalDevice(VkPhysicalDevice physicalDevice_, VkSurfaceKHR surface_,
                      const std::vector<const char *> &deviceExtensions);
  void Wait() { vkDeviceWaitIdle(device_); }
  void Sync();
  void Submit(const VkCommandBuffer *pCommandBuffer, VkSwapchainKHR swapchain,
              uint32_t imageIndex);
};

} // namespace Vulkan
