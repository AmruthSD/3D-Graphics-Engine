#pragma once
#include <camera.hpp>
#include <common.hpp>
#include <terrain.hpp>

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
  VkImageView createImageView(VkImage image, VkFormat format,
                              VkImageAspectFlags aspectFlags,
                              uint32_t mipLevels);
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

  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory);
  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);
  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> inFlightFences;
  void createSyncObjects();

  uint32_t currentFrame = 0;

  unordered_map<Chunk, VkBuffer, ChunkHash> vertexBuffers;
  unordered_map<Chunk, VkDeviceMemory, ChunkHash> vertexBufferMemories;
  void createVertexBuffer(Chunk);

  void createDescriptorSetLayout();
  VkDescriptorSetLayout descriptorSetLayout;

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;
  std::vector<void *> uniformBuffersMapped;
  void createUniformBuffers();
  void updateUniformBuffer(uint32_t currentFrame);

  VkDescriptorPool descriptorPool;
  std::vector<VkDescriptorSet> descriptorSets;
  void createDescriptorPool();
  void createDescriptorSets();

  void recreateSwapChain();
  void cleanupSwapChain();

  void createTextureImage();
  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkImageView textureImageView;
  void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
                   VkFormat format, VkImageTiling tiling,
                   VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                   VkImage &image, VkDeviceMemory &imageMemory);
  void transitionImageLayout(VkImage image, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout,
                             uint32_t mipLevels);
  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                         uint32_t height);
  void createTextureImageView();
  VkSampler textureSampler;
  void createTextureSampler();

  VkCommandBuffer beginSingleTimeCommands();
  void endSingleTimeCommands(VkCommandBuffer commandBuffer);

  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;
  void createDepthResources();
  VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                               VkImageTiling tiling,
                               VkFormatFeatureFlags features);
  VkFormat findDepthFormat();

  uint32_t mipLevels;
  void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth,
                       int32_t texHeight, uint32_t mipLevels);

  Terrain *terrain;

public:
  bool framebufferResized = false;
  GLFWwindow *window;
  WindowHandler();
  void startWindow();
  ~WindowHandler();

  Camera *camera;
};