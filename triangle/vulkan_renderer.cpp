#include "vulkan_renderer.h"
#include "vulkan_swapchain.h"

namespace Vulkan {

std::shared_ptr<Renderer>
Renderer::CreateCommandPool(VkDevice device, VkPhysicalDevice physicalDevice,
                            VkSurfaceKHR surface) {
  auto queueFamilyIndices =
      Vulkan::QueueFamilyIndices::FindQueueFamilies(physicalDevice, surface);

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

  auto ptr = std::shared_ptr<Renderer>(new Renderer(device));
  if (vkCreateCommandPool(device, &poolInfo, nullptr, &ptr->commandPool_) !=
      VK_SUCCESS) {
    // throw std::runtime_error("failed to create command pool!");
    return nullptr;
  }

  // ptr->CreateCommandBuffer();
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = ptr->commandPool_;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  if (vkAllocateCommandBuffers(device, &allocInfo, &ptr->commandBuffer_) !=
      VK_SUCCESS) {
    // throw std::runtime_error("failed to allocate command buffers!");
    return nullptr;
  }

  return ptr;
}

const VkCommandBuffer *Renderer::Render(VkRenderPass renderPass,
                                        VkFramebuffer framebuffer,
                                        VkExtent2D extent,
                                        VkPipeline pipeline) {
  vkResetCommandBuffer(commandBuffer_, /*VkCommandBufferResetFlagBits*/ 0);
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffer_, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = renderPass;
  renderPassInfo.framebuffer = framebuffer;
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = extent;

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(commandBuffer_, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);

  vkCmdBindPipeline(commandBuffer_, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)extent.width;
  viewport.height = (float)extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer_, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = extent;
  vkCmdSetScissor(commandBuffer_, 0, 1, &scissor);

  vkCmdDraw(commandBuffer_, 3, 1, 0, 0);

  vkCmdEndRenderPass(commandBuffer_);

  if (vkEndCommandBuffer(commandBuffer_) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }

  return &commandBuffer_;
}

} // namespace Vulkan