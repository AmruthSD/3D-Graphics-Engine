#include "camera.hpp"
#include "common.hpp"

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

class WindowHandler {
  // vulkan
  const int MAX_FRAMES_IN_FLIGHT = 2;
  std::vector<VkCommandBuffer> commandBuffers;
  const bool enableValidationLayers = true;
  const std::vector<const char *> validationLayers = {
      "VK_LAYER_KHRONOS_validation"};
  VkInstance instance;
  VkDevice device;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  VkSurfaceKHR surface;
  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::vector<VkImageView> swapChainImageViews;
  VkPipelineLayout pipelineLayout;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  const std::vector<const char *> deviceExtensions = {
      VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  VkRenderPass renderPass;
  VkPipeline graphicsPipeline;
  std::vector<VkFramebuffer> swapChainFramebuffers;
  VkCommandPool commandPool;

  void initVulkan();
  void pickDevice();
  void logicalDevice();
  void createSwapChain();
  bool checkDeviceExtensionSupport(VkPhysicalDevice);
  bool isDeviceSuitable(VkPhysicalDevice);
  void createImageViews();
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
  void createGraphicsPipeline();
  VkShaderModule createShaderModule(const std::vector<char> &code);
  void createRenderPass();
  void createFramebuffers();
  void createCommandPool();
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
  VkCommandBuffer commandBuffer;
  void createCommandBuffer();
  void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  void drawFrame();
  bool checkValidationLayerSupport();

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  void createSyncObjects();

  uint32_t currentFrame = 0;

public:
  GLFWwindow *window;
  WindowHandler();
  void startWindow();
  ~WindowHandler();

  Camera *camera;
};