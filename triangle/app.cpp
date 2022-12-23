#include "app.h"
#include "vulkan_device.h"
#include "vulkan_instance.h"
#include "vulkan_pipeline.h"
#include "vulkan_swapchain.h"
#include <vulkan/vulkan_core.h>

const int MAX_FRAMES_IN_FLIGHT = 2;

static const std::vector<const char *> deviceExtensions_ = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

class Impl {

public:
  Impl() {}
  ~Impl() {
    device_->Wait();
    cleanup();
  }

  bool initialize(const char **extensions, size_t size,
                  const GetSurface &getSurface, bool enableValidationLayers) {
    instance_ =
        Vulkan::Instance::Create(extensions, size, enableValidationLayers);
    if (!instance_) {
      return false;
    }

    int w, h;
    surface_ = getSurface(instance_->handle, &w, &h);

    physicalDevice_ = Vulkan::PickPhysicalDevice(instance_->handle, surface_,
                                                 deviceExtensions_);
    device_ = Vulkan::Device::CreateLogicalDevice(physicalDevice_, surface_,
                                                  deviceExtensions_);
    swapChain_ = Vulkan::SwapChain::CreateSwapChain(
        device_->device_, physicalDevice_, surface_, w, h);

    pipeline_ = Vulkan::Pipeline::CreateGraphicsPipeline(
        device_->device_, swapChain_->renderPass_);

    createCommandPool();
    createCommandBuffer();
    createSyncObjects();

    return true;
  }

  void drawFrame() {
    vkWaitForFences(device_->device_, 1, &inFlightFence_, VK_TRUE, UINT64_MAX);
    vkResetFences(device_->device_, 1, &inFlightFence_);

    auto imageIndex =
        swapChain_->AcquireNextImageIndex(imageAvailableSemaphore_);

    vkResetCommandBuffer(commandBuffer_, /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(commandBuffer_, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore_};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer_;

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore_};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(device_->graphicsQueue_, 1, &submitInfo,
                      inFlightFence_) != VK_SUCCESS) {
      throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapChain_->swapChain_};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(device_->presentQueue_, &presentInfo);
  }

private:
  std::shared_ptr<Vulkan::Instance> instance_;
  VkSurfaceKHR surface_;
  VkPhysicalDevice physicalDevice_;
  std::shared_ptr<Vulkan::Device> device_;
  std::shared_ptr<Vulkan::SwapChain> swapChain_;
  std::shared_ptr<Vulkan::Pipeline> pipeline_;

  VkCommandPool commandPool_;
  VkCommandBuffer commandBuffer_;
  VkSemaphore imageAvailableSemaphore_;
  VkSemaphore renderFinishedSemaphore_;
  VkFence inFlightFence_;

  void cleanup() {
    vkDestroySemaphore(device_->device_, renderFinishedSemaphore_, nullptr);
    vkDestroySemaphore(device_->device_, imageAvailableSemaphore_, nullptr);
    vkDestroyFence(device_->device_, inFlightFence_, nullptr);
    vkDestroyCommandPool(device_->device_, commandPool_, nullptr);

    pipeline_ = nullptr;
    swapChain_ = nullptr;
    device_ = nullptr;

    vkDestroySurfaceKHR(instance_->handle, surface_, nullptr);
  }

  void createCommandPool() {
    auto queueFamilyIndices = Vulkan::QueueFamilyIndices::FindQueueFamilies(
        physicalDevice_, surface_);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(device_->device_, &poolInfo, nullptr,
                            &commandPool_) != VK_SUCCESS) {
      throw std::runtime_error("failed to create command pool!");
    }
  }

  void createCommandBuffer() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool_;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device_->device_, &allocInfo,
                                 &commandBuffer_) != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate command buffers!");
    }
  }

  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapChain_->renderPass_;
    renderPassInfo.framebuffer = swapChain_->swapChainFramebuffers_[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChain_->swapChainExtent_;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      pipeline_->graphicsPipeline_);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChain_->swapChainExtent_.width;
    viewport.height = (float)swapChain_->swapChainExtent_.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChain_->swapChainExtent_;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }

  void createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(device_->device_, &semaphoreInfo, nullptr,
                          &imageAvailableSemaphore_) != VK_SUCCESS ||
        vkCreateSemaphore(device_->device_, &semaphoreInfo, nullptr,
                          &renderFinishedSemaphore_) != VK_SUCCESS ||
        vkCreateFence(device_->device_, &fenceInfo, nullptr, &inFlightFence_) !=
            VK_SUCCESS) {
      throw std::runtime_error(
          "failed to create synchronization objects for a frame!");
    }
  }
};

///
/// HelloTriangleApplication
///
HelloTriangleApplication::HelloTriangleApplication() : impl_(new Impl) {}
HelloTriangleApplication::~HelloTriangleApplication() { delete impl_; }
bool HelloTriangleApplication::initialize(const char **extensions, size_t size,
                                          const GetSurface &getSurface,
                                          bool enableValidationLayers) {
  return impl_->initialize(extensions, size, getSurface,
                           enableValidationLayers);
}
void HelloTriangleApplication::drawFrame() { impl_->drawFrame(); }
