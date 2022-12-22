#include "app.h"
#include <iostream>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class AppWindow {
  GLFWwindow *window_ = nullptr;

public:
  ~AppWindow() {
    glfwDestroyWindow(window_);
    glfwTerminate();
  }

  bool create(int width, int height, const char *title) {
    if (!glfwInit()) {
      return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window_) {
      return false;
    }

    return true;
  }

  VkSurfaceKHR createSurface(VkInstance instance) {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, window_, nullptr, &surface) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface!");
      return {};
    }
    return surface;
  }

  bool newFrame() {
    if (glfwWindowShouldClose(window_)) {
      return false;
    }
    glfwPollEvents();
    return true;
  }

  void getBufferSize(int *w, int *h) { glfwGetFramebufferSize(window_, w, h); }
};

std::vector<const char *> getRequiredExtensions(bool enableValidationLayers) {
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  std::vector<const char *> extensions(glfwExtensions,
                                       glfwExtensions + glfwExtensionCount);

  if (enableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  return extensions;
}

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

int main() {
  AppWindow window;

  if (!window.create(WIDTH, HEIGHT, "Vulkan")) {
    return 1;
  }

  HelloTriangleApplication app;

  auto extensions = getRequiredExtensions(enableValidationLayers);

  auto getSurface = [&w = window](VkInstance instance, int *width,
                                  int *height) {
    w.getBufferSize(width, height);
    return w.createSurface(instance);
  };

  if (!app.initialize(extensions.data(), extensions.size(), getSurface, enableValidationLayers)) {
    return 1;
  }

  try {
    while (window.newFrame()) {
      app.drawFrame();
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
