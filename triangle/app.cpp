#include "app.h"
#include "vulkan_device.h"
#include "vulkan_instance.h"
#include "vulkan_pipeline.h"
#include "vulkan_renderer.h"
#include "vulkan_swapchain.h"
#include <memory>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

static const std::vector<const char *> deviceExtensions_ = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

class Impl {
  std::shared_ptr<Vulkan::Instance> instance_;
  VkSurfaceKHR surface_;
  VkPhysicalDevice physicalDevice_;
  std::shared_ptr<Vulkan::Device> device_;
  std::shared_ptr<Vulkan::SwapChain> swapChain_;
  std::shared_ptr<Vulkan::Pipeline> pipeline_;
  std::shared_ptr<Vulkan::Renderer> renderer_;

public:
  Impl() {}
  ~Impl() {
    device_->Wait();
    renderer_ = nullptr;
    pipeline_ = nullptr;
    swapChain_ = nullptr;
    device_ = nullptr;
    vkDestroySurfaceKHR(instance_->handle, surface_, nullptr);
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

    renderer_ = Vulkan::Renderer::CreateCommandPool(device_->device_,
                                                    physicalDevice_, surface_);

    return true;
  }

  void drawFrame() {
    device_->Sync();

    auto imageIndex =
        swapChain_->AcquireNextImageIndex(device_->imageAvailableSemaphore_);

    auto pCommandBuffer = renderer_->Render(
        swapChain_->renderPass_, swapChain_->swapChainFramebuffers_[imageIndex],
        swapChain_->swapChainExtent_, pipeline_->graphicsPipeline_);

    device_->Submit(pCommandBuffer, swapChain_->swapChain_, imageIndex);
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
