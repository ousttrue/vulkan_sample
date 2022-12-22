#pragma once
#include <memory>
#include <vulkan/vulkan.h>

namespace Vulkan {
class Instance {
public:
  VkInstance handle = nullptr;
  ~Instance();

  static std::shared_ptr<Instance> Create(const char **extensions, size_t size,
                                          bool enableValidationLayers);
};
void populateDebugMessengerCreateInfo(
    VkDebugUtilsMessengerCreateInfoEXT &createInfo);

} // namespace Vulkan
