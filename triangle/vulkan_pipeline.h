#pragma once
#include <memory>
#include <vulkan/vulkan.h>

namespace Vulkan {
class Pipeline {
  VkDevice device_;
  VkPipelineLayout pipelineLayout_;

  Pipeline(VkDevice device) : device_(device) {}

public:
  VkPipeline graphicsPipeline_;
  ~Pipeline();
  static std::shared_ptr<Pipeline>
  CreateGraphicsPipeline(VkDevice device, VkRenderPass renderPass);
};

} // namespace Vulkan