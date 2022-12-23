#pragma once
#include <memory>
#include <vulkan/vulkan.h>

namespace Vulkan {

class Renderer {
  VkDevice device_;
  VkCommandPool commandPool_;
  Renderer(VkDevice device) : device_(device) {}

public:
  VkCommandBuffer commandBuffer_;
  ~Renderer() { vkDestroyCommandPool(device_, commandPool_, nullptr); }
  static std::shared_ptr<Renderer>
  CreateCommandPool(VkDevice device, VkPhysicalDevice physicalDevice,
                    VkSurfaceKHR surface);
  const VkCommandBuffer *Render(VkRenderPass renderPass,
                                VkFramebuffer framebuffer, VkExtent2D extent,
                                VkPipeline pipeline);
};

} // namespace Vulkan
