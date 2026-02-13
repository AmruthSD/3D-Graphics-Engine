#include <window.hpp>

void WindowHandler::updateTriangleBuffer(const std::vector<TriangleGPU> &tris) {
  VkDeviceSize newSize = sizeof(TriangleGPU) * tris.size();

  if (newSize == 0)
    return;

  if (newSize > triangleBufferSize) {
    vkDeviceWaitIdle(device);

    vkDestroyBuffer(device, triangleBuffer, nullptr);
    vkFreeMemory(device, triangleBufferMemory, nullptr);

    createBuffer(newSize,
                 VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                     VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, triangleBuffer,
                 triangleBufferMemory);

    triangleBufferSize = newSize;

    updateComputeDescriptorBuffers();
  }

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingMemory;

  createBuffer(triangleBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               stagingBuffer, stagingMemory);

  void *data;
  vkMapMemory(device, stagingMemory, 0, triangleBufferSize, 0, &data);

  memcpy(data, tris.data(), triangleBufferSize);

  vkUnmapMemory(device, stagingMemory);

  copyBuffer(stagingBuffer, triangleBuffer, triangleBufferSize);

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingMemory, nullptr);
}

void WindowHandler::updateBVHBuffer(const std::vector<BVHNodeGPU> &nodes) {
  VkDeviceSize newSize = sizeof(BVHNodeGPU) * nodes.size();

  if (newSize == 0)
    return;

  if (newSize > bvhBufferSize) {
    vkDeviceWaitIdle(device);

    vkDestroyBuffer(device, bvhBuffer, nullptr);
    vkFreeMemory(device, bvhBufferMemory, nullptr);

    createBuffer(
        newSize,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bvhBuffer, bvhBufferMemory);

    bvhBufferSize = newSize;

    updateComputeDescriptorBuffers();
  }

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingMemory;

  createBuffer(bvhBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
               stagingBuffer, stagingMemory);

  void *data;
  vkMapMemory(device, stagingMemory, 0, bvhBufferSize, 0, &data);

  memcpy(data, nodes.data(), bvhBufferSize);

  vkUnmapMemory(device, stagingMemory);

  copyBuffer(stagingBuffer, bvhBuffer, bvhBufferSize);

  vkDestroyBuffer(device, stagingBuffer, nullptr);
  vkFreeMemory(device, stagingMemory, nullptr);
}

void WindowHandler::updateComputeUniformBuffer() {
  ComputeUBO ubo{};

  float aspect = static_cast<float>(width) / static_cast<float>(height);

  glm::mat4 proj = camera->getProgectionMatrix(aspect);

  glm::mat4 view = camera->getView();

  ubo.invViewProj = glm::inverse(proj * view);

  glm::vec3 camPos = camera->getPosition();

  ubo.cameraPos = glm::vec4(camPos, 1.0f);

  glm::vec3 sunDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));

  ubo.lightDir = glm::vec4(sunDir, 0.0f);

  ubo.resolution = glm::vec2(width, height);

  void *data;
  vkMapMemory(device, computeUniformBufferMemory, 0, sizeof(ComputeUBO), 0,
              &data);

  memcpy(data, &ubo, sizeof(ComputeUBO));

  vkUnmapMemory(device, computeUniformBufferMemory);
}

void WindowHandler::createResultImage() {
  VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT;

  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = swapChainExtent.width;
  imageInfo.extent.height = swapChainExtent.height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = format;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = VK_IMAGE_USAGE_STORAGE_BIT | // compute write
                    VK_IMAGE_USAGE_SAMPLED_BIT;  // fragment read
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(device, &imageInfo, nullptr, &resultImage) != VK_SUCCESS)
    throw std::runtime_error("failed to create result image!");

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device, resultImage, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(
      memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  if (vkAllocateMemory(device, &allocInfo, nullptr, &resultImageMemory) !=
      VK_SUCCESS)
    throw std::runtime_error("failed to allocate result image memory!");

  vkBindImageMemory(device, resultImage, resultImageMemory, 0);

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = resultImage;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = format;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  if (vkCreateImageView(device, &viewInfo, nullptr, &resultImageView) !=
      VK_SUCCESS)
    throw std::runtime_error("failed to create result image view!");

  transitionImageLayout(resultImage, format, VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_GENERAL, 1);
}