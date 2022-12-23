#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include <set>
#include <vector>

namespace Vulkan {

std::shared_ptr<Device>
Device::CreateLogicalDevice(VkPhysicalDevice physicalDevice_,
                            VkSurfaceKHR surface_,
                            const std::vector<const char *> &deviceExtensions) {
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

  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  if (vkCreateSemaphore(ptr->device_, &semaphoreInfo, nullptr,
                        &ptr->imageAvailableSemaphore_) != VK_SUCCESS ||
      vkCreateSemaphore(ptr->device_, &semaphoreInfo, nullptr,
                        &ptr->renderFinishedSemaphore_) != VK_SUCCESS ||
      vkCreateFence(ptr->device_, &fenceInfo, nullptr, &ptr->inFlightFence_) !=
          VK_SUCCESS) {
    // throw std::runtime_error(
    //     "failed to create synchronization objects for a frame!");
    return nullptr;
  }

  return ptr;
}

void Device::Sync() {
  vkWaitForFences(device_, 1, &inFlightFence_, VK_TRUE, UINT64_MAX);
  vkResetFences(device_, 1, &inFlightFence_);
}

void Device::Submit(const VkCommandBuffer *pCommandBuffer,
                    VkSwapchainKHR swapchain, uint32_t imageIndex) {
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {imageAvailableSemaphore_};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = pCommandBuffer;

  VkSemaphore signalSemaphores[] = {renderFinishedSemaphore_};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(graphicsQueue_, 1, &submitInfo, inFlightFence_) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo{};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {swapchain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  vkQueuePresentKHR(presentQueue_, &presentInfo);
}

} // namespace Vulkan
