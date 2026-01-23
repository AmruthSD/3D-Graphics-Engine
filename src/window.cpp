#include <window.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

WindowHandler::WindowHandler() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Engine", nullptr, nullptr);

  //   uint32_t extensionCount = 0;
  //   vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
  //   nullptr); vector<VkExtensionProperties> extensions(extensionCount);
  //   vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
  //                                          extensions.data());

  //   std::cout << extensionCount << " extensions supported\n";
  //   for (auto i : extensions) {
  //     cout << i.extensionName << "\n";
  //   }

  initVulkan();
  camera = new Camera();
}

void WindowHandler::startWindow() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    drawFrame();
  }

  vkDeviceWaitIdle(device);
}

WindowHandler::~WindowHandler() {
  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
    vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
    vkDestroyFence(device, inFlightFences[i], nullptr);
  }

  vkDestroyCommandPool(device, commandPool, nullptr);

  for (auto framebuffer : swapChainFramebuffers) {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
  }

  vkDestroyPipeline(device, graphicsPipeline, nullptr);
  vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
  vkDestroyRenderPass(device, renderPass, nullptr);

  vkDestroySwapchainKHR(device, swapChain, nullptr);

  vkDestroyDevice(device, nullptr);
  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyInstance(instance, nullptr);

  glfwDestroyWindow(window);
  glfwTerminate();

  delete camera;
}