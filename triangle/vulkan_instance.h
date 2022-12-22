#pragma once
#include <memory>
#include <vulkan/vulkan.h>

namespace Vulkan {
class Instance {
  bool enableValidationLayers = false;
  VkDebugUtilsMessengerEXT debugMessenger = {};

public:
  VkInstance handle = nullptr;
  ~Instance();
  static std::shared_ptr<Instance> Create(const char **extensions, size_t size,
                                          bool enableValidationLayers);
};

} // namespace Vulkan
