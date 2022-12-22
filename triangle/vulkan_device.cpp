#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include <vector>
#include <set>

namespace Vulkan {

std::shared_ptr<Device>
Device::CreateLogicalDevice(VkPhysicalDevice physicalDevice_,
                            VkSurfaceKHR surface_, const std::vector<const char*> &deviceExtensions) {
  auto indices =
      Vulkan::QueueFamilyIndices::FindQueueFamilies(physicalDevice_, surface_);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                            indices.presentFamily.value()};

  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();

  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  // if (enableValidationLayers) {
  //   createInfo.enabledLayerCount =
  //       static_cast<uint32_t>(validationLayers.size());
  //   createInfo.ppEnabledLayerNames = validationLayers.data();
  // } else
  { createInfo.enabledLayerCount = 0; }

  auto ptr = std::shared_ptr<Device>(new Device);
  if (vkCreateDevice(physicalDevice_, &createInfo, nullptr, &ptr->device_) !=
      VK_SUCCESS) {
    // throw std::runtime_error("failed to create logical device!");
    return nullptr;
  }

  vkGetDeviceQueue(ptr->device_, indices.graphicsFamily.value(), 0,
                   &ptr->graphicsQueue_);
  VkQueue presentQueue_;
  vkGetDeviceQueue(ptr->device_, indices.presentFamily.value(), 0,
                   &ptr->presentQueue_);
  return ptr;
}

} // namespace Vulkan
