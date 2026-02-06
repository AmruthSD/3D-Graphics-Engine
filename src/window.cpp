#include <window.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

static void framebufferResizeCallback(GLFWwindow *window, int width,
                                      int height) {
  auto app =
      reinterpret_cast<WindowHandler *>(glfwGetWindowUserPointer(window));
  app->framebufferResized = true;
}

WindowHandler::WindowHandler() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  ;

  window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Engine", nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
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
  float deltaTime = 0.0f;
  float lastFrame = glfwGetTime();

  const double targetFrameTime = 1.0 / 60.0;

  while (!glfwWindowShouldClose(window)) {
    double frameStart = glfwGetTime();

    glfwPollEvents();

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    camera->processInput(window, deltaTime);

    drawFrame();

    // ---- FPS LIMIT ----
    double frameEnd = glfwGetTime();
    double frameDuration = frameEnd - frameStart;

    if (frameDuration < targetFrameTime) {
      double sleepTime = targetFrameTime - frameDuration;
      std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
    }
  }

  vkDeviceWaitIdle(device);
}

WindowHandler::~WindowHandler() {
  cleanupSwapChain();

  vkDestroySampler(device, textureSampler, nullptr);
  vkDestroyImageView(device, textureImageView, nullptr);
  vkDestroyImage(device, textureImage, nullptr);
  vkFreeMemory(device, textureImageMemory, nullptr);

  vkDestroyPipeline(device, graphicsPipeline, nullptr);
  vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
  vkDestroyRenderPass(device, renderPass, nullptr);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroyBuffer(device, uniformBuffers[i], nullptr);
    vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
  }

  vkDestroyDescriptorPool(device, descriptorPool, nullptr);

  vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

  vkDestroyBuffer(device, indexBuffer, nullptr);
  vkFreeMemory(device, indexBufferMemory, nullptr);

  vkDestroyBuffer(device, vertexBuffer, nullptr);
  vkFreeMemory(device, vertexBufferMemory, nullptr);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
    vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
    vkDestroyFence(device, inFlightFences[i], nullptr);
  }

  vkDestroyCommandPool(device, commandPool, nullptr);

  vkDestroyDevice(device, nullptr);

  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyInstance(instance, nullptr);

  glfwDestroyWindow(window);

  glfwTerminate();
  delete camera;
}